/*
    Developer: Alon Kalif
    File:      minion_cmd_data.cpp
    Reviewer:  
    Date: 	   27.01.2025
    Status:    Implementation done
    Version:   1.0
*/

#include "minion_cmd_data.hpp"

namespace ilrd
{

/* =========================== Read Operation Data ========================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

ReadOperationData::ReadOperationData(uint64_t offset, uint32_t numBytesToRead, Uid uid, MasterProxy* masterProxy) :
m_offset(offset), m_numBytesToRead(numBytesToRead), m_uid(uid), m_masterProxy(masterProxy)
{
    // MIL
}

int ReadOperationData::GetKey()
{
    return READ_OPERATION_CREATOR;
}

uint64_t ReadOperationData::GetOffset() const
{
    return m_offset;
}

uint32_t ReadOperationData::GetNumBytesToRead() const
{
    return m_numBytesToRead;
}

Uid ReadOperationData::GetUid() const
{
    return m_uid;
}

MasterProxy* ReadOperationData::GetMasterProxy() const
{
    return m_masterProxy;
}

/* ========================== Write Operation Data ========================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

WriteOperationData::WriteOperationData(uint64_t offset, uint32_t numBytesToWrite,
                std::shared_ptr<char> data, Uid uid, MasterProxy* masterProxy) :
                m_offset(offset), m_numBytesToWrite(numBytesToWrite), m_dataToWrite(data),
                m_uid(uid), m_masterProxy(masterProxy)
{
    // MIL
}

int WriteOperationData::GetKey()
{
    return WRITE_OPERATION_CREATOR;
}

uint64_t WriteOperationData::GetOffset() const
{
    return m_offset;
}

uint32_t WriteOperationData::GetNumBytesToWrite() const
{
    return m_numBytesToWrite;
}

std::shared_ptr<char> WriteOperationData::GetData() const
{   
    return m_dataToWrite;
}

Uid WriteOperationData::GetUid() const
{
    return m_uid;
}

MasterProxy* WriteOperationData::GetMasterProxy() const
{
    return m_masterProxy;
}

} // namespace ilrd





