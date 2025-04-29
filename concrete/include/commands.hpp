/*
    Developer: Alon Kalif
    File:      commands.hpp
    Reviewer:  
    Date: 	   24.01.2025
    Status:    Tested
    Version:   1.0
*/

#ifndef ILRD_RD161_COMMANDS_HPP
#define ILRD_RD161_COMMANDS_HPP

#include <vector>

#include <nlohmann/json.hpp>    // To handle json config file

#include "ICommand.hpp"         // To inherit from ICommand
#include "cmd_data.hpp"         // For command-data containers
#include "cpp_uid.hpp"          // For Uid type
#define __HANDLETON__
#include "raid_manager.hpp"     // For RaidManager::MinionAccessPoint type
#undef __HANDLETON__

namespace ilrd
{

class ReadCommand : public ICommand<int>
{
public:
	std::pair< std::function<bool()>, std::chrono::milliseconds>
	    Run(std::shared_ptr<ICommandData<int>> cmdData) override; // may throw invalid_argument

    static std::shared_ptr<ICommand<int>> Creator ();
    static void Config (const nlohmann::json& config);


private:
    static size_t s_checkResponseInterval;
    static size_t s_checkResponseThreshold;

    void             InvokeMinionProxy (const std::vector<RaidManager::MinionAccessPoint>&, ReadCommandData*, const Uid&);
    ReadCommandData* ExtractDataPtr (std::shared_ptr<ICommandData<int>> cmdData);
    static void      NoResponseHandler ();

    class ResponseChecker
    {
    public:
        ResponseChecker(size_t, Uid);

        bool operator()();
    private:
        size_t m_threshold;
        Uid    m_cmdUid;
    };
};

class WriteCommand : public ICommand<int>
{
public:
	std::pair< std::function<bool()>, std::chrono::milliseconds>
	    Run(std::shared_ptr<ICommandData<int>> cmdData) override; // may throw invalid_argument

    static void Config (const nlohmann::json& config, size_t numBackups);
    static std::shared_ptr<ICommand<int>> Creator ();

private:
    static size_t s_checkResponseInterval;
    static size_t s_checkResponseThreshold;
    static size_t s_numBackups;

    void              InvokeMinionProxy (const std::vector<RaidManager::MinionAccessPoint>&, WriteCommandData*, const Uid&);
    WriteCommandData* ExtractDataPtr(std::shared_ptr<ICommandData<int>> cmdData);
    static void       NoResponseHandler ();

    class ResponseChecker
    {
    public:
        ResponseChecker(size_t, Uid);

        bool operator()();
    private:
        size_t m_threshold;
        Uid    m_cmdUid;
    };
};

} // namespace ilrd

#endif	// ILRD_RD161_COMMANDS_HPP 
