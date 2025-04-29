/*
    Developer: Alon Kalif
    File:      communication_lib.cpp
    Reviewer:  
    Date: 	   12.01.2025
    Status:    In progress
    Version:   1.1 TCPManager
*/

#include <sys/socket.h>             // For socket()   

#include "communication_lib.hpp"	
#include "logger.hpp"               // For documentation
#include "concrete_messages.hpp"    // Messages for logger 

namespace ilrd
{

extern Logger* g_logger;

static const int DEFAULT_PROTOCOL    =  0;
static const int CL_SUCCESS          =  0;
static const int CL_ERROR            = -1;
static const int NO_FLAGS            =  0;

/* =============================== SocketAddr =============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

SocketAddr::SocketAddr(const std::string& ip, const std::string& port) :
m_ip(ip), m_port(port)
{
    struct sockaddr_in* addr_in = reinterpret_cast<struct sockaddr_in*>(&m_addr);   // cast to Internet domain sockets

    addr_in->sin_family = AF_INET;
    addr_in->sin_port = htons(std::stoi(m_port));                 // converts to network byte order (big endian)
    
    if(0 == inet_pton(AF_INET, m_ip.c_str(), &addr_in->sin_addr)) // converts string to network address
    {
        g_logger->Log(SOCKET_ADDR_MSG::SOCKET_ADDR_INIT_ERROR, __FILE__, __LINE__);
        throw std::invalid_argument(SOCKET_ADDR_MSG::SOCKET_ADDR_INIT_ERROR);
    }
}

struct sockaddr* SocketAddr::GetAddr() const
{
    return const_cast<struct sockaddr*>(&m_addr);
}

socklen_t SocketAddr::GetStructLen() const
{
    return sizeof(m_addr);
}

std::string SocketAddr::GetIpStr() const
{
    return m_ip;
}

std::string SocketAddr::GetPortStr() const
{
    return m_port;
}

/* =============================== UDP Socket =============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

UdpSocket::UdpSocket(const std::string& ip, const std::string& port) : m_sockAddr(ip, port)
{
    m_fd = socket(AF_INET, SOCK_DGRAM, DEFAULT_PROTOCOL);

    if(CL_ERROR == m_fd)
    {
        g_logger->Log(UDP_SOCKET_MSG::SOCKET_INIT_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
        throw std::runtime_error(UDP_SOCKET_MSG::SOCKET_INIT_ERROR);
    }

    if(CL_ERROR == bind(m_fd, m_sockAddr.GetAddr(), m_sockAddr.GetStructLen()))
    {
        close(m_fd);
        g_logger->Log(UDP_SOCKET_MSG::SOCKET_BIND_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
        throw std::runtime_error(UDP_SOCKET_MSG::SOCKET_BIND_ERROR);
    }
}

UdpSocket::~UdpSocket()
{
    close(m_fd);
}

size_t UdpSocket::SendMsg(const SocketAddr& dest, const void* msg, size_t msgLen)
{
    ssize_t bytesSent = sendto(m_fd, msg, msgLen, 0, dest.GetAddr(), dest.GetStructLen());

    if(CL_ERROR == bytesSent)
    {
        g_logger->Log(UDP_SOCKET_MSG::SOCKET_SEND_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
        throw std::runtime_error(UDP_SOCKET_MSG::SOCKET_SEND_ERROR);
    }

    return bytesSent;
}

size_t UdpSocket::RecvMsg(void* buffer_out, size_t buffSize, int flags, SocketAddr* srcAddr_out)
{
    socklen_t len         = 0;
    socklen_t* pLen       = nullptr;
    struct sockaddr* addr = nullptr;

    if(nullptr != srcAddr_out)
    {
        addr = srcAddr_out->GetAddr();
        len = srcAddr_out->GetStructLen();
        pLen = &len;
    }

    ssize_t bytesReceived = recvfrom(m_fd, buffer_out, buffSize, flags, addr, pLen);

    if(CL_ERROR == bytesReceived)
    {
        g_logger->Log(UDP_SOCKET_MSG::SOCKET_RECV_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
        throw std::runtime_error(UDP_SOCKET_MSG::SOCKET_RECV_ERROR);
    }

    return bytesReceived;
}

int UdpSocket::GetFd()
{
    return m_fd;
}

/* =============================== TCP Manager ============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

const int TcpManager::LISTEN_QUEUE_SIZE   = 10;
const int TcpManager::CLIENT_DISCONNECTED = 0;

TcpManager::TcpManager(const std::string& ip, const std::string& port) : m_listenerAddr(ip, port)
{
    m_fdListener = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);

    if(CL_ERROR == m_fdListener)
    {
        g_logger->Log(TCP_MANAGER_MSG::SOCKET_INIT_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__); 
        throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_INIT_ERROR);
    }

    if(CL_ERROR == bind(m_fdListener, m_listenerAddr.GetAddr(), m_listenerAddr.GetStructLen()))
    {
        close(m_fdListener);
        g_logger->Log(TCP_MANAGER_MSG::SOCKET_BIND_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__); 
        throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_BIND_ERROR);
    }

    if(CL_ERROR == listen(m_fdListener, LISTEN_QUEUE_SIZE))
    {
        close(m_fdListener);
        g_logger->Log(TCP_MANAGER_MSG::SOCKET_LISTEN_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__); 
        throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_LISTEN_ERROR);
    }
}

size_t TcpManager::SendMsg(const void* msg, size_t msgLen, int clientFd)
{
    ssize_t bytesSent = 0;

    if(m_clients.contains(clientFd))
    {
        bytesSent = send(clientFd, msg, msgLen, 0);
        if(CL_ERROR == bytesSent)
        {
            g_logger->Log(TCP_MANAGER_MSG::SOCKET_SEND_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__); 
            throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_SEND_ERROR);
        }
    }

    return bytesSent;
}

size_t TcpManager::RecvMsg(void* buffer_out, size_t buffSize, int clientFd)
{
    ssize_t bytesReceived = 0;

    if(m_clients.contains(clientFd))
    {
        bytesReceived = recv(clientFd, buffer_out, buffSize, 0);
        if(CL_ERROR == bytesReceived)
        {
            g_logger->Log(TCP_MANAGER_MSG::SOCKET_RECV_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
            throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_RECV_ERROR);
        }

        if(CLIENT_DISCONNECTED == bytesReceived)
        {
            m_clients.erase(clientFd);
            #ifndef NDEBUG
                g_logger->Log(TCP_MANAGER_MSG::CLIENT_DISCONNECTED);
            #endif
        }
    }

    return bytesReceived;
}

int TcpManager::GetFd()
{
    return m_fdListener;
}

std::unordered_set<int> TcpManager::GetClients()
{
    return m_clients;
}

int TcpManager::AcceptTcpClient()
{
    int newClientFd = accept(m_fdListener, NULL, NULL);

    if(CL_ERROR == newClientFd)
    {
        g_logger->Log(TCP_MANAGER_MSG::SOCKET_ACCEPT_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__); 
        throw std::runtime_error(TCP_MANAGER_MSG::SOCKET_ACCEPT_ERROR);
    }

    m_clients.insert(newClientFd);

    #ifndef NDEBUG
        g_logger->Log(TCP_MANAGER_MSG::CLIENT_ACCEPTED);
    #endif

    return newClientFd;
}

} // namespace ilrd





