/*
    Developer: Alon Kalif
    File:      master-minion_messages.cpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    In progress
    Version:   1.0
*/


#include <cstring>      // For memcpy

#include "master-minion_messages.hpp"
#include "logger.hpp"                   // For documentation
#include "concrete_messages.hpp"        // Messages for logger 

namespace ilrd
{

extern Logger* g_logger;

/* ================================== AMsg ================================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

AMsg::~AMsg() = default;

AMsg::AMsg(MsgType msgType, const Uid& uid) : m_msgType(msgType), m_uid(uid)
{
    ATransmittable::UpdateBufferSize(GetObjSize());
}

char* AMsg::ToBuffer(char* buffer) const
{
    buffer = ATransmittable::ToBuffer(buffer);

    *(int32_t*)buffer = m_msgType;
    buffer += sizeof(int32_t);

    buffer = m_uid.ToBuffer(buffer);

    return buffer;
}

char* AMsg::FromBuffer(char* buffer)
{
    buffer = ATransmittable::FromBuffer(buffer);

    m_msgType = *(int32_t*)buffer;
    buffer += sizeof(int32_t);

    buffer = m_uid.FromBuffer(buffer);

    return buffer;
}

uint32_t AMsg::GetObjSize() const
{
    return sizeof(int32_t) + m_uid.GetObjSize();
}

AMsg::MsgType AMsg::GetMsgType() const
{
    return static_cast<MsgType>(m_msgType);
}

Uid AMsg::GetUid() const
{
    return m_uid;
}

/* ================================ ACmdMsg ================================= */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

ACmdMsg::~ACmdMsg() = default;

ACmdMsg::ACmdMsg(MsgType msgType, const Uid& uid, uint64_t offset, uint32_t numBytes)
    : AMsg(msgType, uid), m_offset(offset), m_numBytes(numBytes)
{
    ATransmittable::UpdateBufferSize(GetObjSize());
}

uint32_t ACmdMsg::GetObjSize() const
{
    return sizeof(uint64_t) + sizeof(uint32_t);
}

char* ACmdMsg::ToBuffer(char* buffer) const
{
    buffer = AMsg::ToBuffer(buffer);

    *(uint64_t*)buffer = m_offset;
    buffer += sizeof(uint64_t);

    *(uint32_t*)buffer = m_numBytes;
    buffer += sizeof(uint32_t);

    return buffer;
}

char* ACmdMsg::FromBuffer(char* buffer)
{
    buffer = AMsg::FromBuffer(buffer);

    m_offset = *(uint64_t*)buffer;
    buffer += sizeof(uint64_t);

    m_numBytes = *(uint32_t*)buffer;
    buffer += sizeof(uint32_t);

    return buffer;
}

uint64_t ACmdMsg::GetOffset() const
{
    return m_offset;
}

uint32_t ACmdMsg::GetNumBytes() const
{
    return m_numBytes;
}

/* =============================== ReadCmdMsg =============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

ReadCmdMsg::ReadCmdMsg(const Uid& uid, uint64_t offset, uint32_t numBytesToRead)
    : ACmdMsg(READ_CMD, uid, offset, numBytesToRead)
{
    #ifndef NDEBUG
        g_logger->Log(TRANSMITTABLE_MSG::NEW_READ_CMD);
    #endif
}

std::shared_ptr<AMsg> ReadCmdMsg::Creator()
{
    return std::make_shared<ReadCmdMsg>();
}

/* =============================== WriteCmdMsg ============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

WriteCmdMsg::WriteCmdMsg(const Uid& uid, uint64_t offset, uint32_t numBytesToWrite, std::shared_ptr<char> dataToWrite)
    : ACmdMsg(WRITE_CMD, uid, offset, numBytesToWrite), m_dataToWrite(dataToWrite)
{
    ATransmittable::UpdateBufferSize(numBytesToWrite);

    #ifndef NDEBUG
        g_logger->Log(TRANSMITTABLE_MSG::NEW_WRITE_CMD);
    #endif
}

char* WriteCmdMsg::ToBuffer(char* buffer) const
{
    buffer = ACmdMsg::ToBuffer(buffer);

    std::memcpy(buffer, m_dataToWrite.get(), ACmdMsg::GetNumBytes());
    
    return buffer + ACmdMsg::GetNumBytes();
}

char* WriteCmdMsg::FromBuffer(char* buffer)
{
    buffer = ACmdMsg::FromBuffer(buffer);

    std::shared_ptr<char> tmp(new char[ACmdMsg::GetNumBytes()], std::default_delete<char[]>());

    std::memcpy(tmp.get(), buffer, ACmdMsg::GetNumBytes());
    
    m_dataToWrite = tmp;

    return buffer + ACmdMsg::GetNumBytes();
}

void WriteCmdMsg::SetDataToWrite(std::shared_ptr<char> dataToWrite)
{
    m_dataToWrite = dataToWrite;
}

std::shared_ptr<char> WriteCmdMsg::GetDataToWrite() const
{
    return m_dataToWrite;
}

std::shared_ptr<AMsg> WriteCmdMsg::Creator()
{
    return std::make_shared<WriteCmdMsg>();
}

/* =============================== ResponseMsg ============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

ResponseMsg::ResponseMsg(MsgType msgType, const Uid& uid, ResponseStatus status, uint32_t responseLen, std::shared_ptr<char> responseData)
    : AMsg(msgType, uid), m_status(status), m_responseLen(responseLen), m_responseData(responseData)
{
    ATransmittable::UpdateBufferSize(GetObjSize());

    #ifndef NDEBUG
        g_logger->Log(TRANSMITTABLE_MSG::NEW_RESPONSE);
    #endif
}

char* ResponseMsg::ToBuffer(char* buffer) const
{
    buffer = AMsg::ToBuffer(buffer);

    *(int32_t*)buffer = m_status;
    buffer += sizeof(int32_t);

    *(uint32_t*)buffer = m_responseLen;
    buffer += sizeof(uint32_t);

    if(FAILURE == m_status)
    {
        return buffer;
    }

    std::memcpy(buffer, m_responseData.get(), m_responseLen);

    return buffer + m_responseLen;  
}

char* ResponseMsg::FromBuffer(char* buffer)
{
    buffer = AMsg::FromBuffer(buffer);

    m_status = *(int32_t*)buffer;
    buffer += sizeof(int32_t);

    m_responseLen = *(uint32_t*)buffer;
    buffer += sizeof(uint32_t);

    if(FAILURE == m_status)
    {
        return buffer;
    }

    std::shared_ptr<char> tmp(new char[m_responseLen], std::default_delete<char[]>());

    std::memcpy(tmp.get(), buffer, m_responseLen);

    m_responseData = tmp;

    return buffer + m_responseLen;
}

uint32_t ResponseMsg::GetObjSize() const
{
    return sizeof(int32_t) + sizeof(uint32_t) + m_responseLen;
}

ResponseMsg::ResponseStatus ResponseMsg::GetStatus() const
{
    return static_cast<ResponseStatus>(m_status);
}

uint32_t ResponseMsg::GetResponseLen() const
{
    return m_responseLen;
}

std::shared_ptr<char> ResponseMsg::GetResponseData() const
{
    return m_responseData;
}

std::shared_ptr<AMsg> ResponseMsg::Creator()
{
    return std::make_shared<ResponseMsg>();
}

} // namespace ilrd





