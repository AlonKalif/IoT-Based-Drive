/*
    Developer: Alon Kalif
    File:      response_manager.cpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    tested
    Version:   1.1 thread-safe
*/

#include "response_manager.hpp"
#include "logger.hpp"               // For documentation
#include "concrete_messages.hpp"    // Messages for logger 

namespace ilrd
{

extern Logger* g_logger;

Uid ResponseManager::RegisterCommand(size_t numResponses, uint64_t nbdId)
{
    #ifndef NDEBUG
        g_logger->Log(RESPONSE_MANAGER_MSG::CMD_REGISTERED);
    #endif

    std::scoped_lock<std::mutex> lock(m_mapLock);
    return m_watchList.emplace(Uid(), std::make_pair(numResponses, nbdId)).first->first; // 
}

uint64_t ResponseManager::RegisterResponse(const Uid& uid)
{
    uint64_t nbdId = 0;

    std::scoped_lock<std::mutex> lock(m_mapLock);

    if(m_watchList.contains(uid))
    {
        if((0 == (--(m_watchList.at(uid).first))))
        {
            nbdId = m_watchList.at(uid).second;
            m_watchList.erase(uid);
        }
        #ifndef NDEBUG
            g_logger->Log(RESPONSE_MANAGER_MSG::RESPONSE_REGISTERED);
        #endif
    }
    else
    {
        g_logger->Log(RESPONSE_MANAGER_MSG::REGISTER_RESPONSE_FAIL, __FILE__, __LINE__);
    }

    return nbdId;
}

bool ResponseManager::GetCommandStatus(const Uid& uid)
{
    std::scoped_lock<std::mutex> lock(m_mapLock);

    return !(m_watchList.contains(uid));
}

} // namespace ilrd





