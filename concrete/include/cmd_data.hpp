/*
    Developer: Alon Kalif
    File:      cmd_data.hpp
    Reviewer:  
    Date: 	   11.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_CMD_DATA_HPP
#define ILRD_RD161_CMD_DATA_HPP

#include <memory>           // For shared_ptr

#include "ICommandData.hpp" // To inherit from ICommandData

namespace ilrd
{

enum FactoryKey
{
    READ_CMD_CREATOR,
    WRITE_CMD_CREATOR
};

class ReadCommandData : public ICommandData<int>
{
public:
    ReadCommandData(uint64_t offset, uint32_t numBytesToRead, uint64_t nbdId);

    int        GetKey()            override;
    uint64_t   GetOffset()         const;
    uint32_t   GetNumBytesToRead() const;
    uint64_t   GetNbdId()          const;

private:
    uint64_t m_offset;
    uint32_t m_numBytesToRead;
    uint64_t m_nbdId;
};

class WriteCommandData : public ICommandData<int>
{
public:
    WriteCommandData(uint64_t offset, uint32_t numBytesToWrite, std::shared_ptr<char> data, uint64_t nbdId);

    int                   GetKey()             override;
    uint64_t              GetOffset()          const;
    uint32_t              GetNumBytesToWrite() const;
    std::shared_ptr<char> GetData()            const;
    uint64_t              GetNbdId()           const;
    
private:
    uint64_t              m_offset;
    uint32_t              m_numBytesToWrite;
    std::shared_ptr<char> m_dataToWrite;
    uint64_t              m_nbdId;
};

} // namespace ilrd

#endif	// ILRD_RD161_CMD_DATA_HPP 
