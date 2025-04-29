/*
    Developer: Alon Kalif
    File:      commands.cpp
    Reviewer:  
    Date: 	   24.01.2025
    Status:    Tested
    Version:   1.0
*/

#include "commands.hpp"
#include "logger.hpp"            // For documentation
#include "concrete_messages.hpp" // Messages for logger 
#include "response_manager.hpp"  // To register commands 
#include "minion_proxy.hpp"      // To invoke minions
#include "response_manager.hpp"  // For GetResponseStatus()

namespace ilrd
{

extern Logger* g_logger;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Helper Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void GetAccessPoints(uint64_t offset, std::vector<RaidManager::MinionAccessPoint>* accessPoints_out)
{
    std::pair<RaidManager::MinionAccessPoint, RaidManager::MinionAccessPoint> accessPoints;

    try
    { 
        accessPoints = Singleton<RaidManager>::GetInstance()->OffsetToProxy(offset);
    }
    catch(const std::invalid_argument& e)
    {
        g_logger->Log(COMMAND_MSG::OFFSET_ERROR, __FILE__, __LINE__);
        // how to handle?
    }

    accessPoints_out->push_back(accessPoints.first);
    accessPoints_out->push_back(accessPoints.second);
}

Uid RegisterToResponseManager(size_t numResponses, uint64_t nbdId)
{
    return Singleton<ResponseManager>::GetInstance()->RegisterCommand(numResponses, nbdId);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Read Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

size_t ReadCommand::s_checkResponseInterval  = 0; // Initialized from Config
size_t ReadCommand::s_checkResponseThreshold = 0; // Initialized from Config

std::pair< std::function<bool()>, std::chrono::milliseconds >
ReadCommand::Run(std::shared_ptr<ICommandData<int>> cmdData)
{
    ReadCommandData* readCmdData = ExtractDataPtr(cmdData);

    std::vector<RaidManager::MinionAccessPoint> accessPoints;

    GetAccessPoints(readCmdData->GetOffset(), &accessPoints);

    Uid cmdUid = RegisterToResponseManager(1, readCmdData->GetNbdId());

    InvokeMinionProxy(accessPoints, readCmdData, cmdUid);

    return std::make_pair(ResponseChecker(s_checkResponseThreshold, cmdUid), std::chrono::milliseconds(s_checkResponseInterval));
}	

void ReadCommand::Config(const nlohmann::json& config)
{
    ReadCommand::s_checkResponseInterval = config.at("read_cmd_check_response_interval_in_millisec").get<size_t>();
    ReadCommand::s_checkResponseThreshold = config.at("read_cmd_check_response_threshold").get<size_t>();
}

void ReadCommand::InvokeMinionProxy(const std::vector<RaidManager::MinionAccessPoint>& accessPoints,
                                    ReadCommandData* cmdData, const Uid& cmdUid)
{
    // Invoke minion proxy's Read(offset, numBytes, uid)
    accessPoints.begin()->first->Read(accessPoints.begin()->second, cmdData->GetNumBytesToRead(), cmdUid);
}

ReadCommandData* ReadCommand::ExtractDataPtr(std::shared_ptr<ICommandData<int>> cmdData)
{
    ReadCommandData* readCmdData = dynamic_cast<ReadCommandData*>(cmdData.get());

    if(nullptr == readCmdData)
    {
        g_logger->Log(COMMAND_MSG::READ_CMD_ARG_ERROR, __FILE__, __LINE__);
        throw std::invalid_argument(COMMAND_MSG::READ_CMD_ARG_ERROR);
    }

    return readCmdData;
}

std::shared_ptr<ICommand<int>> ReadCommand::Creator()
{
	return std::make_shared<ReadCommand>();
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~ Read Response Checker ~~~~~~~~~~~~~~~~~~~~~~~~~ */

ReadCommand::ResponseChecker::ResponseChecker(size_t threshold, Uid cmdUid) 
    : m_threshold(threshold), m_cmdUid(cmdUid)
{
    // MIL
}

bool ReadCommand::ResponseChecker::operator()()
{
    if(0 != Singleton<ResponseManager>::GetInstance()->GetCommandStatus(m_cmdUid))
    {
        return true;
    }

    if(0 == m_threshold--)
    {
        ReadCommand::NoResponseHandler();
    }

    return false;
}

void ReadCommand::NoResponseHandler()
{
    // retransmit
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Write Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

size_t WriteCommand::s_checkResponseInterval  = 0; // Initialized from Config
size_t WriteCommand::s_checkResponseThreshold = 0; // Initialized from Config
size_t WriteCommand::s_numBackups             = 0; // Initialized from Config

std::pair< std::function<bool()>, std::chrono::milliseconds >
WriteCommand::Run(std::shared_ptr<ICommandData<int>> cmdData)
{
    WriteCommandData* writeCmdData = ExtractDataPtr(cmdData);

    std::vector<RaidManager::MinionAccessPoint> accessPoints;

    GetAccessPoints(writeCmdData->GetOffset(), &accessPoints);

    Uid cmdUid = RegisterToResponseManager(s_numBackups, writeCmdData->GetNbdId());

    InvokeMinionProxy(accessPoints, writeCmdData, cmdUid);

    return std::make_pair(ResponseChecker(s_checkResponseThreshold, cmdUid), std::chrono::milliseconds(s_checkResponseInterval));
}

void WriteCommand::Config(const nlohmann::json& config, size_t numBackups)
{
    WriteCommand::s_checkResponseInterval  = config.at("write_cmd_check_response_interval_in_millisec");
    WriteCommand::s_checkResponseThreshold = config.at("write_cmd_check_response_threshold");
    WriteCommand::s_numBackups             = numBackups;
}

void WriteCommand::InvokeMinionProxy(const std::vector<RaidManager::MinionAccessPoint>& accessPoints,
                                     WriteCommandData* cmdData, const Uid& cmdUid)
{
    for(auto& iter : accessPoints)
    {
        // Invoke minion proxy's Write(offset, numBytes, dataToWrite, uid)
        iter.first->Write(iter.second, cmdData->GetNumBytesToWrite(), cmdData->GetData(), cmdUid);
    }
}

WriteCommandData* WriteCommand::ExtractDataPtr(std::shared_ptr<ICommandData<int>> cmdData)
{
    WriteCommandData* writeCmdData = dynamic_cast<WriteCommandData*>(cmdData.get());
    if(nullptr == writeCmdData)
    {
        g_logger->Log(COMMAND_MSG::WRITE_CMD_ARG_ERROR, __FILE__, __LINE__);
        throw std::invalid_argument(COMMAND_MSG::WRITE_CMD_ARG_ERROR);
    }

    return writeCmdData;
}

std::shared_ptr<ICommand<int>> WriteCommand::Creator()
{
    return std::make_shared<WriteCommand>();
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~ Write Response Checker ~~~~~~~~~~~~~~~~~~~~~~~~~ */

WriteCommand::ResponseChecker::ResponseChecker(size_t threshold, Uid cmdUid) 
    : m_threshold(threshold), m_cmdUid(cmdUid)
{
    // MIL
}

bool WriteCommand::ResponseChecker::operator()()
{
    if(0 != Singleton<ResponseManager>::GetInstance()->GetCommandStatus(m_cmdUid))
    {
        return true;
    }

    if(0 == m_threshold--)
    {
        WriteCommand::NoResponseHandler();
    }

    return false;
}

void WriteCommand::NoResponseHandler()
{
    // retransmit
}

} // namespace ilrd





