/*
    Developer: Alon Kalif
    File:      nbd_proxy.cpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.1
*/

#include <cstdlib>          // For system()
#include <cstring>          // For memcpy
#include <cassert>          // For assert

#include <linux/nbd.h>  // For NBD structs & macros
#include <sys/ioctl.h>  // For ioctl()
#include <sys/stat.h>   // For open()
#include <fcntl.h>      // For open()
#include <arpa/inet.h>  // For htonl()

#include "nbd_proxy.hpp"
#include "logger.hpp"              // For documentation
#include "concrete_messages.hpp"   // Messages for logger 	
#include "cmd_data.hpp"            // Container for NBD data
#include "master_side.hpp"         // To call MasterSide::Stop()

#ifdef WORDS_BIGENDIAN
    uint64_t ntohll(uint64_t a) 
    {
        return a;
    }
#else
    uint64_t ntohll(uint64_t a) 
    {
        uint32_t lo = a & 0xffffffff;
        uint32_t hi = a >> 32U;
        lo = ntohl(lo);
        hi = ntohl(hi);
        return ((uint64_t) lo) << 32U | hi;
    }
#endif

namespace ilrd
{

extern Logger* g_logger;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NBD Proxy ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

NBDProxy::NBDProxy(std::string nbdDevicePath, size_t totalDriveMem) 
    : m_nbdDevicePath(nbdDevicePath), m_totalDriveMem(totalDriveMem)
{
    InitNbd();
}

NBDProxy::~NBDProxy()
{
    close(m_nbdServerSock);
    close(m_nbdClientSock);
    
    if (m_nbdClientThread.joinable()) 
    {
        m_nbdClientThread.join();
    }
    
    close(m_nbdDeviceFd);
}

std::shared_ptr<ICommandData<int>> NBDProxy::GetCommandData(int fd, Reactor<int>::Mode mode)
{
    (void)mode; // unused
    (void)fd;   // unused

    std::shared_ptr<ICommandData<int>> cmdData = nullptr;

    struct nbd_request request;

    {
        std::scoped_lock<std::mutex> lock(m_sockLock);

        ssize_t bytesRead = read(m_nbdServerSock, &request, sizeof(request));
        assert(0 < bytesRead);
    }

    assert(request.magic == htonl(NBD_REQUEST_MAGIC));

    uint64_t nbdId = 0;
    memcpy(&nbdId, request.handle, 8);

    switch(ntohl(request.type))
    {
        case NBD_CMD_READ:
        {
            #ifndef NDEBUG
                g_logger->Log(NBD_PROXY_MSG::RECEIVED_READ_REQUEST);
            #endif

            cmdData = CreateReadData(ntohll(request.from), ntohl(request.len), nbdId);
        }
        break;

        case NBD_CMD_WRITE:
        {
            #ifndef NDEBUG
                g_logger->Log(NBD_PROXY_MSG::RECEIVED_WRITE_REQUEST);
            #endif

            cmdData = CreateWriteData(ntohll(request.from), ntohl(request.len), nbdId);
        }
        break;

        default:
            g_logger->Log(NBD_PROXY_MSG::MSG_TYPE_ERROR, __FILE__, __LINE__);
            return nullptr;
    } 
    
    return cmdData;
}

std::shared_ptr<ICommandData<int>> NBDProxy::CreateReadData(uint64_t offset, uint32_t numBytes, uint64_t nbdId)
{
    return std::make_shared<ReadCommandData>(offset, numBytes, nbdId);
}

std::shared_ptr<ICommandData<int>> NBDProxy::CreateWriteData(uint64_t offset, uint32_t numBytes, uint64_t nbdId)
{
    std::shared_ptr<char> dataToWrite(new char[numBytes], std::default_delete<char[]>());

    ReadAll(m_nbdServerSock, dataToWrite.get(), numBytes);

    return std::make_shared<WriteCommandData>(offset, numBytes, dataToWrite, nbdId);
}

void NBDProxy::InitNbd()
{
    m_nbdDeviceFd = open(m_nbdDevicePath.c_str(), O_RDWR);
    if(-1 == m_nbdDeviceFd)
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdSetup - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    m_disconnector = std::make_shared<NBDProxy::NBDDisconnector>(m_nbdDeviceFd);

    if(-1 == ioctl(m_nbdDeviceFd, NBD_CLEAR_QUE))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdClientSetup - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    if(-1 == ioctl(m_nbdDeviceFd, NBD_CLEAR_SOCK))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdClientSetup - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    if(-1 == ioctl(m_nbdDeviceFd, NBD_SET_SIZE, m_totalDriveMem))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdSetup - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    int connectedSocketPair[2];

    if(-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, connectedSocketPair))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdClientSetup - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }
  
    m_nbdServerSock = connectedSocketPair[0];
    m_nbdClientSock = connectedSocketPair[1];     

    m_nbdClientThread = std::thread(NBDProxy::NbdClientDoIt, m_nbdDeviceFd, m_nbdClientSock);
}

void NBDProxy::NbdClientDoIt(int m_nbdDeviceFd, int m_nbdClientSock)
{
    if(-1 == ioctl(m_nbdDeviceFd, NBD_SET_SOCK, m_nbdClientSock))
    {
        // how to handle?
        throw std::runtime_error("ERROR - NbdMediator::Start - line "  + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    } 

    // Blocking - Does kernel magic and sends TCP messages
    int status = ioctl(m_nbdDeviceFd, NBD_DO_IT);
    if(-1 == status)
    {
        // how to handle?
        throw std::runtime_error("ERROR - NbdMediator::Start - line "  + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    // Clean up:
    if((-1 == ioctl(m_nbdDeviceFd, NBD_CLEAR_QUE)))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdClientDoIt - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));
    }

    if((-1 == ioctl(m_nbdDeviceFd, NBD_CLEAR_SOCK)))
    {
        throw std::runtime_error("ERROR - NBDProxy::NbdClientDoIt - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno));        
    }
}

void NBDProxy::SendResponseToNbd(std::shared_ptr<char> responseContent, uint32_t responseLen, AMsg::MsgType msgType, uint64_t nbdId)
{
    struct nbd_reply reply;

    reply.magic = htonl(NBD_REPLY_MAGIC);
    reply.error = htonl(0);
    assert(0 != nbdId);

    memcpy(reply.handle, &nbdId, sizeof(reply.handle));
    
    switch (msgType)
    {
        case AMsg::MsgType::READ_RESPONSE:
        
            WriteAll(m_nbdServerSock, (char*)&reply, sizeof(struct nbd_reply));
            WriteAll(m_nbdServerSock, responseContent.get(), responseLen);

        break;

        case AMsg::MsgType::WRITE_RESPONSE:
        
            WriteAll(m_nbdServerSock, (char*)&reply, sizeof(struct nbd_reply));
            
        break;
        
        default:
        
            g_logger->Log(NBD_PROXY_MSG::MSG_TYPE_ERROR, __FILE__, __LINE__);
            reply.error = -1;
            WriteAll(m_nbdServerSock, (char*)&reply, sizeof(struct nbd_reply));
        
        break;
    }
}

void NBDProxy::ReadAll(int fd, char* buffer, uint32_t numBytes)
{
    std::scoped_lock<std::mutex> lock(m_sockLock);

    int bytesRead = 0;

    while (numBytes > 0) 
    {
        bytesRead = read(fd, buffer, numBytes);
        assert(bytesRead > 0);
        buffer += bytesRead;
        numBytes -= bytesRead;
    }

    assert(numBytes == 0);
}

void NBDProxy::WriteAll(int fd, char* buffer, uint32_t numBytes)
{
    std::scoped_lock<std::mutex> lock(m_sockLock);
    
    int bytesWritten;

    while (numBytes > 0) 
    {
        bytesWritten = write(fd, buffer, numBytes);
        assert(bytesWritten > 0);
        buffer += bytesWritten;
        numBytes -= bytesWritten;
    }

    assert(numBytes == 0);
}

std::shared_ptr<IInputProxy<int, int>> NBDProxy::GetNbdDisconnectorProxy() const
{
    return m_disconnector;
}

int NBDProxy::GetServerFd() const 
{
    return m_nbdServerSock;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~ Nested - NBD Disconnector ~~~~~~~~~~~~~~~~~~~~~~~ */

NBDProxy::NBDDisconnector::NBDDisconnector(int nbdDeviceFd) : m_nbdDeviceFd(nbdDeviceFd)
{
    // MIL
}

std::shared_ptr<ICommandData<int>> NBDProxy::NBDDisconnector::GetCommandData(int fd, Reactor<int>::Mode mode)
{
    (void)mode; // unused

    int bytesRead = 0;

    char buffer[MAX_INPUT] = {0};

    bytesRead = read(fd, buffer, MAX_INPUT);
    assert(bytesRead > 0);

    if('q' == buffer[0])
    {
        if(-1 == ioctl(m_nbdDeviceFd, NBD_DISCONNECT)) 
        {
            std::cerr << "ERROR - NBDProxy::DisconnectHandler - line " + std::to_string(__LINE__) + " - errno = " + strerror(errno) << std::endl;
        }

        Singleton<MasterSide>::GetInstance()->Stop();
    }    

    return nullptr;
}

} // namespace ilrd





