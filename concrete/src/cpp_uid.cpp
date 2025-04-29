/*
    Developer: Alon Kalif
    File:      cpp_uid.hpp
    Reviewer:  
    Date: 	   24.01.2025
    Status:    Tested
    Version:   1.1 
*/

#include <iostream>              // Temp, for testing
#include <functional>            // For std::hash

#include "cpp_uid.hpp"	
#include "logger.hpp"            // For documentation
#include "concrete_messages.hpp" // Messages for logger

namespace ilrd
{

extern Logger* g_logger;

Uid::Uid() : m_uid(UIDCreate())
{
    if(UIDIsSame(m_uid, UIDBadUID()))
    {
        g_logger->Log(UID_MSG::UID_CREATE_ERROR, __FILE__, __LINE__);
        throw std::runtime_error(UID_MSG::UID_CREATE_ERROR);
    }

    ATransmittable::UpdateBufferSize(GetObjSize());
}

size_t Uid::HashUid::operator()(const Uid& uid) const
{
    size_t timeHash = std::hash<time_t> ()(uid.m_uid.time_stamp);
    size_t pidHash  = std::hash<pid_t>  ()(uid.m_uid.pid_stamp);
    size_t keyHash  = std::hash<size_t> ()(uid.m_uid.key);

    return (timeHash ^ (pidHash << 1) ^ (keyHash << 2));
}

char* Uid::ToBuffer(char* buffer) const
{
    *(uint64_t*)buffer = m_uid.time_stamp;
    buffer += sizeof(uint64_t);

    *(int32_t*)buffer = m_uid.pid_stamp;
    buffer += sizeof(int32_t);

    *(uint64_t*)buffer = m_uid.key;
    buffer += sizeof(uint64_t);

    return buffer;
}

char* Uid::FromBuffer(char* buffer)      
{
    m_uid.time_stamp = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    m_uid.pid_stamp = *(int32_t*)buffer;
    buffer += sizeof(int32_t);

    m_uid.key = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    return buffer;
}

uint32_t Uid::GetObjSize() const
{
    return  sizeof(uint64_t) +
            sizeof(int32_t)  +
            sizeof(uint64_t);
}

bool operator==(const Uid& one, const Uid& other)
{
    return UIDIsSame(one.m_uid, other.m_uid);
}

bool operator!=(const Uid& one, const Uid& other)
{
    return !(one == other);
}

bool operator<(const Uid& one, const Uid& other)
{
    return one.m_uid.key < other.m_uid.key;
}

} // namespace ilrd





