/*
    Developer: Alon Kalif
    File:      minion_cmd_data.hpp
    Reviewer:  
    Date: 	   27.01.2025
    Status:    Implementation done
    Version:   1.0
*/

#ifndef ILRD_RD161_MINION_CMD_DATA_HPP
#define ILRD_RD161_MINION_CMD_DATA_HPP

#include <memory>   // For shared_ptr

#include "ICommandData.hpp"     // To inherit from ICommandData
#include "cpp_uid.hpp"          // For class Uid
#include "master_proxy.hpp"     // To pass master proxy's pointer to r/w operations

namespace ilrd
{

enum MinionFactoryKey
{
    READ_OPERATION_CREATOR,
    WRITE_OPERATION_CREATOR
};

class ReadOperationData : public ICommandData<int>
{
public:
    ReadOperationData(uint64_t offset, uint32_t numBytesToRead, Uid uid, MasterProxy* masterProxy);

    int          GetKey()             override;
    uint64_t     GetOffset()          const;
    uint32_t     GetNumBytesToRead()  const;
    Uid          GetUid()             const;
    MasterProxy* GetMasterProxy()     const;

private:
    uint64_t     m_offset;
    uint32_t     m_numBytesToRead;
    Uid          m_uid;
    MasterProxy* m_masterProxy;
};

class WriteOperationData : public ICommandData<int>
{
public:
    WriteOperationData(uint64_t offset, uint32_t numBytesToWrite, std::shared_ptr<char> data, Uid uid, MasterProxy* masterProxy);

    int                   GetKey()             override;
    uint64_t              GetOffset()          const;
    uint32_t              GetNumBytesToWrite() const;
    std::shared_ptr<char> GetData()            const;
    Uid                   GetUid()             const;
    MasterProxy*          GetMasterProxy()     const;
private:
    uint64_t              m_offset;
    uint32_t              m_numBytesToWrite;
    std::shared_ptr<char> m_dataToWrite;
    Uid                   m_uid;
    MasterProxy*          m_masterProxy;
};

} // namespace ilrd

#endif	// ILRD_RD161_MINION_CMD_DATA_HPP 
