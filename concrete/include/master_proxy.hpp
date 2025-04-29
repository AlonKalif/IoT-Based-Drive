/*
    Developer: Alon Kalif
    File:      master_proxy.hpp
    Reviewer:  
    Date: 	   26.01.2025
    Status:    Tested
    Version:   1.0
*/

#ifndef ILRD_RD161_MASTER_PROXY_HPP
#define ILRD_RD161_MASTER_PROXY_HPP

#include <mutex>
#include <string>
#include <memory>   // For shared_ptr

#include "master-minion_messages.hpp"   // For ACmdMsg
#include "communication_lib.hpp"        // For UdpSocket 
#include "IInputProxy.hpp"              // To inherit from IInputProxy
#include "ICommandData.hpp"             // To return ICommandData ptr
namespace ilrd
{

class MasterProxy : public IInputProxy<int, int>
{
public:
    MasterProxy(std::string ip, std::string port);

    std::shared_ptr<ICommandData<int>> GetCommandData(int fd, Reactor<int>::Mode mode) override;

    void SendResponseToMaster(const AMsg& responseMsg);

    int GetMasterProxyFd();

private:
    std::mutex m_sockLock;
    UdpSocket  m_udpSocket;
    SocketAddr m_masterAddr;
    
    std::shared_ptr<ICommandData<int>> MsgToOperationData(std::shared_ptr<ACmdMsg> cmdMsg);
};

} // namespace ilrd

#endif	// ILRD_RD161_MASTER_PROXY_HPP 
