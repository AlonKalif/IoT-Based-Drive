#ifndef ILRD_RD161_CONCRETE_MESSAGES_HPP
#define ILRD_RD161_CONCRETE_MESSAGES_HPP

#include <string>

namespace READ_OP_MSG
{
    const std::string OFFSET_ERROR = "ERROR - ReadOperation::Run - offset exceeds memory size";
    const std::string MEM_READ_ERROR = "ERROR - ReadOperation::Run - failed to read from memory";
    const std::string MEM_READ_SUCCESS = "ReadOperation::Run - successfully read data from memory";
}

namespace WRITE_OP_MSG
{
    const std::string OFFSET_ERROR = "ERROR - WriteOperation::Run - offset exceeds memory size";
    const std::string MEM_WRITE_ERROR = "ERROR - WriteOperation::Run - failed to write to memory";
    const std::string MEM_WRITE_SUCCESS = "WriteOperation::Run - successfully written data into memory";
}

namespace MASTER_PROXY_MSG
{
    const std::string MSG_TYPE_ERROR = "ERROR - MasterProxy::CreateCommandData - received message of unfamiliar type";
    const std::string INIT = "master proxy is up at IP ";
    const std::string INVOKED = "master proxy invoked from fd ";
}

#endif // ILRD_RD161_CONCRETE_MESSAGES_HPP