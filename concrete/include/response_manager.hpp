/*
    Developer: Alon Kalif
    File:      response_manager.hpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    tested
    Version:   1.1 thread-safe
*/

#ifndef ILRD_RD161_RESPONSE_MANAGER_HPP
#define ILRD_RD161_RESPONSE_MANAGER_HPP

#include <unordered_map>
#include <mutex>

#define __HANDLETON__
#include "handleton.hpp"    // To make ResponseManager singleton
#undef __HANDLETON__
#include "cpp_uid.hpp"      // To identify commands

namespace ilrd
{

class ResponseManager
{
public:
    Uid      RegisterCommand (size_t numResponses, uint64_t nbdId); // may throw runtime_error
    uint64_t RegisterResponse(const Uid& uid);                      // returns 0 if waiting for more responds, returns nbd id otherwise
    bool     GetCommandStatus(const Uid& uid);                      // returns false if uid is still waiting for responses
private:
    ResponseManager() = default;
    friend class Singleton<ResponseManager>;

    std::unordered_map<Uid, std::pair<size_t, uint64_t>, Uid::HashUid> m_watchList;
    std::mutex m_mapLock;
};

} // namespace ilrd

#endif	// ILRD_RD161_RESPONSE_MANAGER_HPP 
