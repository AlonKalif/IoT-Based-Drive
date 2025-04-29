/*
    Developer: Alon Kalif
    File:      cmd_data.hpp
    Reviewer:  
    Date: 	   11.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "cmd_data.hpp"

namespace ilrd
{

/* ================================== Read ================================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

ReadCommandData::ReadCommandData(uint64_t offset, uint32_t numBytesToRead, uint64_t nbdId) :
m_offset(offset), m_numBytesToRead(numBytesToRead), m_nbdId(nbdId)
{
    // MIL
}

int ReadCommandData::GetKey()
{
    return READ_CMD_CREATOR;
}

uint64_t ReadCommandData::GetOffset() const
{
    return m_offset;
}

uint32_t ReadCommandData::GetNumBytesToRead() const
{
    return m_numBytesToRead;
}

uint64_t ReadCommandData::GetNbdId() const
{
    return m_nbdId;
}

/* ================================= Write ================================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

WriteCommandData::WriteCommandData(uint64_t offset, uint32_t numBytesToWrite, std::shared_ptr<char> data, uint64_t nbdId) :
    m_offset(offset), m_numBytesToWrite(numBytesToWrite), m_dataToWrite(data), m_nbdId(nbdId)
{
    // MIL
}

int WriteCommandData::GetKey() 
{
    return WRITE_CMD_CREATOR;
}

uint64_t WriteCommandData::GetOffset() const
{
    return m_offset;
}

uint32_t WriteCommandData::GetNumBytesToWrite() const
{
    return m_numBytesToWrite;
}

std::shared_ptr<char> WriteCommandData::GetData() const
{
    return m_dataToWrite;
}

uint64_t WriteCommandData::GetNbdId() const
{
    return m_nbdId;
}

} // namespace ilrd





