/*
    Developer: Alon Kalif
    File:      minion_operations.cpp
    Reviewer:  
    Date: 	   27.01.2025
    Status:    In progress
    Version:   1.0
*/

#include <fstream>      // For file handling

#include "minion_operations.hpp"
#include "minion_side.hpp"                  // For GetMemFile
#include "logger.hpp"                       // For documentation
#include "logger_messages_minion_side.hpp"  // Messages for logger 

namespace ilrd
{

extern Logger*    g_logger;
static std::mutex g_fileLock;

/* ============================= Read Operation ============================= */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

size_t ReadOperation::s_memSize = 0;   // Init from Config    

void ReadOperation::Config(size_t memSize)
{
    s_memSize = memSize;
}

std::pair< std::function<bool()>, std::chrono::milliseconds>
    ReadOperation::Run(std::shared_ptr<ICommandData<int>> operationData)
{
    ReadOperationData* data = reinterpret_cast<ReadOperationData*>(operationData.get());

    ResponseMsg::ResponseStatus readStatus = ResponseMsg::SUCCESS;

    std::shared_ptr<char> dataRequested = PerformRead(data);

    if(nullptr == dataRequested)
    {
        readStatus = ResponseMsg::FAILURE;
    }

    data->GetMasterProxy()->SendResponseToMaster(ResponseMsg(AMsg::READ_RESPONSE, data->GetUid(), readStatus,
                                                    data->GetNumBytesToRead(), dataRequested));

    return std::make_pair(nullptr, static_cast<std::chrono::milliseconds>(0));
}

std::shared_ptr<char> ReadOperation::PerformRead(ReadOperationData* data)
{
    uint64_t memOffset = data->GetOffset();

    if(memOffset >= s_memSize)
    {
        g_logger->Log(READ_OP_MSG::OFFSET_ERROR + ". offset = " +
                      std::to_string(memOffset), __FILE__, __LINE__);

        return nullptr;
    }

    uint32_t size = data->GetNumBytesToRead();

    std::shared_ptr<char> dataRequested(new char[size], std::default_delete<char[]>());
    
    {
        std::scoped_lock<std::mutex> lock(g_fileLock);
        std::fstream* memory = Singleton<MinionSide>::GetInstance()->GetMemFile();
        memory->seekg(memOffset, std::ios::beg);
        memory->read(dataRequested.get(), size);
    }

    return dataRequested;
}

std::shared_ptr<ICommand<int>> ReadOperation::Creator()
{
    return std::make_shared<ReadOperation>();
}

/* ============================= Write Operation ============================ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

size_t WriteOperation::s_memSize = 0;   // Init from Config

void WriteOperation::Config(size_t memSize)
{
    s_memSize = memSize;
}

std::pair< std::function<bool()>, std::chrono::milliseconds>
    WriteOperation::Run(std::shared_ptr<ICommandData<int>> operationData)
{
    WriteOperationData* data = reinterpret_cast<WriteOperationData*>(operationData.get());

    ResponseMsg::ResponseStatus status = PerformWrite(data);

    std::shared_ptr<char> isSuccess = std::make_shared<char>(status == ResponseMsg::ResponseStatus::SUCCESS ? 1 : 0);

    data->GetMasterProxy()->SendResponseToMaster(ResponseMsg(AMsg::WRITE_RESPONSE, data->GetUid(), status, 1, isSuccess));

    return std::make_pair(nullptr, static_cast<std::chrono::milliseconds>(0));  
}

ResponseMsg::ResponseStatus WriteOperation::PerformWrite(WriteOperationData* data)
{
    uint64_t memOffset = data->GetOffset();

    if(memOffset >= s_memSize)
    {
        g_logger->Log(WRITE_OP_MSG::OFFSET_ERROR + ". offset = " +
                      std::to_string(memOffset), __FILE__, __LINE__);

        return ResponseMsg::ResponseStatus::FAILURE;
    }
    
    {
        std::scoped_lock<std::mutex> lock(g_fileLock);
        std::fstream* memory = Singleton<MinionSide>::GetInstance()->GetMemFile();
        memory->seekp(memOffset, std::ios::beg);
        memory->write(data->GetData().get(), data->GetNumBytesToWrite());
    }

    return ResponseMsg::ResponseStatus::SUCCESS;
}

std::shared_ptr<ICommand<int>> WriteOperation::Creator()
{
    return std::make_shared<WriteOperation>();
}

} // namespace ilrd





