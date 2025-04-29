/*
    Developer: Alon Kalif
    File:      response_proxy.cpp
    Reviewer:  
    Date: 	   30.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "response_proxy.hpp"	      
#include "minion_proxy.hpp"           // For ReadMinionMsg
#include "cpp_uid.hpp"                // For Uid
#include "master-minion_messages.hpp" // For messages class definitions
#include "response_manager.hpp"       // For RegisterResponse
#include "master_side.hpp"            // For GetNbdProxy

namespace ilrd
{

ResponseProxy::ResponseProxy(MinionProxy* minionProxy) : m_minionProxy(minionProxy)
{
    // MIL
}

std::shared_ptr<ICommandData<int>> ResponseProxy::GetCommandData(int fd, Reactor<int>::Mode mode)
{
    (void)mode; (void)fd;   // unused

    std::shared_ptr<ResponseMsg> responseMsg = m_minionProxy->ReceiveMinionMsg();

    if(ResponseMsg::ResponseStatus::FAILURE == responseMsg->GetStatus())
    {
        BadStatusHandler();

        return nullptr;
    }

    uint64_t nbdId = Singleton<ResponseManager>::GetInstance()->RegisterResponse(responseMsg->GetUid());

    if(nbdId != 0)
    {
        Singleton<MasterSide>::GetInstance()->GetNbdProxy()->SendResponseToNbd(
            responseMsg->GetResponseData(), responseMsg->GetResponseLen(), responseMsg->GetMsgType(), nbdId);
    }
    
    return nullptr;
}

void ResponseProxy::BadStatusHandler()
{
    // TODO
}


} // namespace ilrd





