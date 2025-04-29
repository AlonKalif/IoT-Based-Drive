/*
    Developer: Alon Kalif
    File:      minion_proxy.hpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_MINION_PROXY_HPP
#define ILRD_RD161_MINION_PROXY_HPP

#include <mutex>
#include <memory>                       // For shared_ptr

#include "cpp_uid.hpp"                  // To pass uid along with message
#include "communication_lib.hpp"        // For UdpSocket & SocketAddr
#include "master-minion_messages.hpp"   // For ACmdMsg class
#include "IInputProxy.hpp"              // To inherit from IInputProxy
#include "response_proxy.hpp"           // For ResponseProxy class

namespace ilrd
{

class IMinionProxy
{
public:
    virtual void Read  (uint64_t offset, uint32_t numBytes, const Uid&) = 0;
    virtual void Write (uint64_t offset, uint32_t numBytes, std::shared_ptr<char> data, const Uid&) = 0;
};

class MinionProxy : public IMinionProxy
{
public:
    MinionProxy(const std::string& minionIp, const std::string& minionPort,
                const std::string& proxyIp, const std::string& proxyPort);  // may throw runtime_error or invalid_argument

    void Read  (uint64_t offset, uint32_t numBytes, const Uid& uid)                             override; // may throw runtime_error
    void Write (uint64_t offset, uint32_t numBytes, std::shared_ptr<char> data, const Uid& uid) override; // may throw runtime_error

    std::shared_ptr<ResponseMsg> ReceiveMinionMsg();

    int GetSockFd();
    std::shared_ptr<ResponseProxy> GetResponseProxy();

private:

    std::mutex m_sockLock;
    SocketAddr m_minionAddr;
    UdpSocket  m_udpSocket;
    std::shared_ptr<ResponseProxy> m_responseProxy;

    void SendMsg(const ACmdMsg& msg);    
};

} // namespace ilrd

#endif	// ILRD_RD161_MINION_PROXY_HPP 
