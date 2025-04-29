/*
    Developer: Alon Kalif
    File:      communication_lib.hpp
    Reviewer:  
    Date: 	   12.01.2025
    Status:    In progress
    Version:   1.1 TCPManager
*/

#ifndef ILRD_RD161_COMMUNICATION_LIB_HPP
#define ILRD_RD161_COMMUNICATION_LIB_HPP

#include <unordered_set>
#include <string>
        
#include <arpa/inet.h>          // For sockaddr_in

namespace ilrd
{

static const std::string DEFAULT_IP   = "127.0.0.1";
static const std::string DEFAULT_PORT = "4950";

class SocketAddr
{
public:
    explicit SocketAddr() = default;
    explicit SocketAddr(const std::string& ip, const std::string& port); // may throw invalid_argument
    
    struct sockaddr* GetAddr()      const;
    socklen_t        GetStructLen() const;
    std::string      GetIpStr()     const;
    std::string      GetPortStr()   const;
private:
    struct sockaddr m_addr;
    std::string m_ip;
    std::string m_port;
};

class UdpSocket
{
public:
    explicit UdpSocket(const std::string& ip = DEFAULT_IP, const std::string& port = DEFAULT_PORT);   // may throw runtime_error or invalid_argument
    ~UdpSocket();

    size_t SendMsg(const SocketAddr& dest, const void* msg, size_t msgLen); // may throw runtime_error

    /* If you don't care who the sender is, pass nullptr to srcAddr_out */
    size_t RecvMsg(void* buffer_out, size_t buffSize,int flags, SocketAddr* srcAddr_out);  // Blocking, may throw runtime_error

    int GetFd();

private:
    SocketAddr m_sockAddr;
    int m_fd;
};

class TcpManager
{
public:
    explicit TcpManager(const std::string& ip = DEFAULT_IP, const std::string& port = DEFAULT_PORT);   // may throw runtime_error or invalid_argument

    size_t SendMsg(const void* msg, size_t msgLen, int clientFd);    // may throw runtime_error
    size_t RecvMsg(void* buffer_out, size_t buffSize, int clientFd); // Blocking, may throw runtime_error
    int    AcceptTcpClient();                                        // Blocking, may throw runtime_error

    int GetFd();
    std::unordered_set<int> GetClients();
private:
    SocketAddr              m_listenerAddr;
    int                     m_fdListener;
    std::unordered_set<int> m_clients;

    static const int LISTEN_QUEUE_SIZE;
    static const int CLIENT_DISCONNECTED;
};

} // namespace ilrd

#endif	// ILRD_RD161_COMMUNICATION_LIB_HPP 
