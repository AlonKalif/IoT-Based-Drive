/*
    Developer: Alon Kalif
    File:      master_proxy.cpp
    Reviewer:  
    Date: 	   26.01.2025
    Status:    Tested
    Version:   1.0
*/

#include "master_proxy.hpp"
#include "factory.hpp"                      // To create AMsg objects
#include "minion_cmd_data.hpp"              // For read/write data containers 
#include "logger.hpp"                       // For documentation
#include "logger_messages_minion_side.hpp"  // Messages for logger 

namespace ilrd
{

extern Logger*             g_logger;
extern Factory<int, AMsg>* g_amsgFactory;

MasterProxy::MasterProxy(std::string ip, std::string port) : m_udpSocket(ip, port)
{
    #ifndef NDEBUG
        g_logger->Log(MASTER_PROXY_MSG::INIT + ip + " and port " + port);
    #endif
}

std::shared_ptr<ICommandData<int>> MasterProxy::GetCommandData(int fd, Reactor<int>::Mode mode)
{
    (void)fd; (void)mode; // unused    

    char* msgBuffer = nullptr;
    std::shared_ptr<AMsg> cmdMsg = nullptr;
    
    // Receive master's data
    {
        uint64_t buffSizeAndKey;
        
        std::scoped_lock<std::mutex> lock(m_sockLock);

        m_udpSocket.RecvMsg(&buffSizeAndKey, sizeof(uint64_t), MSG_PEEK, &m_masterAddr);

        msgBuffer = new char[static_cast<uint32_t>(buffSizeAndKey)];

        m_udpSocket.RecvMsg(msgBuffer, static_cast<uint32_t>(buffSizeAndKey), 0, nullptr);

        cmdMsg = (g_amsgFactory->Create(static_cast<uint32_t>(buffSizeAndKey >> 32)));   
    }

    cmdMsg->FromBuffer(msgBuffer);

    delete[] msgBuffer;

    return MsgToOperationData(dynamic_pointer_cast<ACmdMsg>(cmdMsg));
}

std::shared_ptr<ICommandData<int>> MasterProxy::MsgToOperationData(std::shared_ptr<ACmdMsg> cmdMsg)
{
    switch (cmdMsg->GetMsgType())
    {
        case AMsg::MsgType::READ_CMD:

            return std::make_shared<ReadOperationData>(cmdMsg->GetOffset(),
                                cmdMsg->GetNumBytes(), cmdMsg->GetUid(), this);
        
        case AMsg::MsgType::WRITE_CMD:

            return std::make_shared<WriteOperationData>(
                cmdMsg->GetOffset(), cmdMsg->GetNumBytes(),
                (reinterpret_cast<WriteCmdMsg*>(cmdMsg.get()))->GetDataToWrite(),
                cmdMsg->GetUid(), this);
            
        default:

            g_logger->Log(MASTER_PROXY_MSG::MSG_TYPE_ERROR, __FILE__, __LINE__);
            return nullptr;
    }
}

void MasterProxy::SendResponseToMaster(const AMsg& responseMsg)
{
    size_t buffSize = responseMsg.GetBufferSize();

    char* buffer = new char[buffSize];

    responseMsg.ToBuffer(buffer);
    
    {
        std::scoped_lock<std::mutex> lock(m_sockLock);

        m_udpSocket.SendMsg(m_masterAddr, buffer, buffSize);
    }
    
    delete[] buffer;
}

int MasterProxy::GetMasterProxyFd()
{   
    return m_udpSocket.GetFd();
}


} // namespace ilrd





