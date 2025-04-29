/*
    Developer: Alon Kalif
    File:      nbd_proxy.hpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.1 
*/

#ifndef ILRD_RD161_NBD_PROXY_HPP
#define ILRD_RD161_NBD_PROXY_HPP

#include <mutex>

#include "IInputProxy.hpp"            // To inherit from IInputProxy
#include "ICommandData.hpp"           // NBD data container
#include "master-minion_messages.hpp" // For AMsg::MsgType

namespace ilrd
{

class NBDProxy : public IInputProxy<int, int>
{
public:
    NBDProxy(std::string nbdDevicePath, size_t totalDriveMem);
   ~NBDProxy();

    std::shared_ptr<ICommandData<int>> GetCommandData(int fd, Reactor<int>::Mode mode) override;

    void SendResponseToNbd(std::shared_ptr<char> responseContent, uint32_t responseLen, AMsg::MsgType, uint64_t nbdId);
    
    std::shared_ptr<IInputProxy<int, int>> GetNbdDisconnectorProxy() const;

    int GetServerFd() const;

private:
    class NBDDisconnector : public IInputProxy<int, int>
    {
    public:
        NBDDisconnector(int nbdDeviceFd);
        std::shared_ptr<ICommandData<int>> GetCommandData(int fd, Reactor<int>::Mode mode) override;
    private:
        int m_nbdDeviceFd;
    };

    std::mutex  m_sockLock;
    std::string m_nbdDevicePath;
    size_t      m_totalDriveMem;
    int         m_nbdDeviceFd;
    int         m_nbdServerSock;
    int         m_nbdClientSock;
    std::thread m_nbdClientThread;
    std::shared_ptr<NBDDisconnector> m_disconnector;

    void InitNbd();
    static void NbdClientDoIt(int, int); // blocking

    std::shared_ptr<ICommandData<int>> CreateReadData(uint64_t offset, uint32_t numBytes, uint64_t nbdId);
    std::shared_ptr<ICommandData<int>> CreateWriteData(uint64_t offset, uint32_t numBytes, uint64_t nbdId);

    void ReadAll(int fd, char* buffer, uint32_t numBytes);
    void WriteAll(int fd, char* buffer, uint32_t numBytes);
};

} // namespace ilrd

#endif	// ILRD_RD161_NBD_PROXY_HPP 
