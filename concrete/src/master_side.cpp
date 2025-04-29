/*
    Developer: Alon Kalif
    File:      master_side.hpp
    Reviewer:  
    Date: 	   11.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "master_side.hpp"
#include "commands.hpp"            // For ReadCommand::Creator, WriteCommand::Creator
#include "on_dll_load_fail.hpp"    // For on-load fail function
#include "raid_manager.hpp"        // For GetMinionProxies ()
#include "factory.hpp"             // To init AMsg Factory

namespace ilrd
{

extern Factory<int, AMsg>* g_amsgFactory;

MasterSide::MasterSide() : m_listener(std::make_shared<SelectListener>())
{
    g_amsgFactory->Add(AMsg::MsgType::READ_RESPONSE,  ResponseMsg::Creator);
    g_amsgFactory->Add(AMsg::MsgType::WRITE_RESPONSE, ResponseMsg::Creator);
}

void MasterSide::Config(const nlohmann::json& config)
{
    m_plugAndPlayPath = config.at("plug_and_play_path") .get<std::string>();

    m_nbdProxy = std::make_shared<NBDProxy>(config.at("nbd_device_path").get<std::string>(), 
                                            config.at("total_drive_mem_in_bytes").get<size_t>());
}

void MasterSide::Run()
{
    InitFramework();
    
    m_framework->Run();
}

void MasterSide::Stop()
{   
    m_framework->Stop();
}

void MasterSide::InitFramework()
{
    std::vector<std::tuple<int, Reactor<int>::Mode, std::shared_ptr<IInputProxy<int, int>>>> fdCallbacks;
    fdCallbacks.emplace_back(m_nbdProxy->GetServerFd(), Reactor<int>::READ, m_nbdProxy);
    fdCallbacks.emplace_back(fileno(stdin)            , Reactor<int>::READ, m_nbdProxy->GetNbdDisconnectorProxy());

    std::vector<std::shared_ptr<MinionProxy>>* minionProxies = 
    Singleton<RaidManager>::GetInstance()->GetMinionProxies();
    for(auto iter : *minionProxies)
    {
        fdCallbacks.emplace_back(iter->GetSockFd(), Reactor<int>::READ, iter->GetResponseProxy());
    }

    std::vector<std::pair<int, std::function<std::shared_ptr<ICommand<int>>()> >> creators;    
    creators.emplace_back(READ_CMD_CREATOR,  ReadCommand::Creator);
    creators.emplace_back(WRITE_CMD_CREATOR, WriteCommand::Creator);
    
    m_framework = std::make_shared<Framework<int, int>>(
        fdCallbacks, creators, m_plugAndPlayPath, m_listener, OnDllLoadFail);
}

std::shared_ptr<NBDProxy> MasterSide::GetNbdProxy() const
{
    return m_nbdProxy;
}

} // namespace ilrd





