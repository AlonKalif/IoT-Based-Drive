/*
    Developer: Alon Kalif
    File:      minion_proxy.cpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#include <linux/nbd.h>  // For NBD structs & macros
#include <sys/ioctl.h>  // For ioctl()
#include <sys/stat.h>   // For open()
#include <fcntl.h>      // For open()
#include <arpa/inet.h>  // For htonl()

#include "minion_proxy.hpp"
#include "logger.hpp"                       // For documentation
#include "logger_messages_minion_side.hpp"  // Messages for logger 
#include "factory.hpp"                      // To create AMsg objects
#include "response_manager.hpp"             // For RegisterResponse

namespace ilrd
{
extern Logger*             g_logger;
extern Factory<int, AMsg>* g_amsgFactory;

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

MinionProxy::MinionProxy(const std::string& minionIp, const std::string& minionPort,
                         const std::string& proxyIp, const std::string& proxyPort) :
                         m_minionAddr(minionIp, minionPort), m_udpSocket(proxyIp, proxyPort),
                         m_responseProxy(std::make_shared<ResponseProxy>(this))
{
    // MIL
}

void MinionProxy::Read(uint64_t offset, uint32_t numBytes, const Uid& uid)
{
    ReadCmdMsg rcm(uid, offset, numBytes);

    SendMsg(rcm);
}

void MinionProxy::Write(uint64_t offset, uint32_t numBytes, std::shared_ptr<char> data, const Uid& uid)
{
    WriteCmdMsg wcm(uid, offset, numBytes, data);

    SendMsg(wcm);
}

void MinionProxy::SendMsg(const ACmdMsg& msg)
{
    uint32_t buffSize = msg.GetBufferSize();

    char* buffer = new char[buffSize];

    msg.ToBuffer(buffer);

    {
        std::scoped_lock<std::mutex> lock(m_sockLock);

        m_udpSocket.SendMsg(m_minionAddr, buffer, buffSize);
    }

    delete[] buffer;
}

std::shared_ptr<ResponseMsg> MinionProxy::ReceiveMinionMsg()
{
    char* msgBuffer = nullptr;
    std::shared_ptr<AMsg> responseMsg = nullptr;

    {
        uint64_t buffSizeAndKey;

        std::scoped_lock<std::mutex> lock(m_sockLock);

        m_udpSocket.RecvMsg(&buffSizeAndKey, sizeof(uint64_t), MSG_PEEK, nullptr);

        msgBuffer = new char[static_cast<uint32_t>(buffSizeAndKey)];

        m_udpSocket.RecvMsg(msgBuffer, static_cast<uint32_t>(buffSizeAndKey), 0, nullptr);

        responseMsg = (g_amsgFactory->Create(static_cast<uint32_t>(buffSizeAndKey >> 32)));
    }

    responseMsg->FromBuffer(msgBuffer);

    delete[] msgBuffer;

    return std::dynamic_pointer_cast<ResponseMsg>(responseMsg);
}

int MinionProxy::GetSockFd()
{
    return m_udpSocket.GetFd();
}

std::shared_ptr<ResponseProxy> MinionProxy::GetResponseProxy()
{
    return m_responseProxy;
}

} // namespace ilrd





