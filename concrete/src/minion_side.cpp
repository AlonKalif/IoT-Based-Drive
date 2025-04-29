/*
    Developer: Alon Kalif
    File:      minion_side.cpp
    Reviewer:  
    Date: 	   29.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "minion_side.hpp"	      
#include "minion_operations.hpp"  // To init ICommand factory with r/w operations
#include "factory.hpp"            // To init AMsg factory

namespace ilrd
{

extern Factory<int, AMsg>* g_amsgFactory;

MinionSide::MinionSide() : m_listener(std::make_shared<SelectListener>()),
    m_shutdownProxy(std::make_shared<MinionSide::ShutdownProxy>())
{
    g_amsgFactory->Add(AMsg::MsgType::READ_CMD,  ReadCmdMsg::Creator);
    g_amsgFactory->Add(AMsg::MsgType::WRITE_CMD, WriteCmdMsg::Creator);
}

MinionSide::~MinionSide()
{
    m_memFile.close();
}

void MinionSide::Config(const std::string& ip, const std::string& port,
                        const std::string& file, const std::string& plugAndPlayPath)
{
    m_minionIp        = ip;
    m_minionPort      = port;
    m_memoryFilePath  = file;
    m_plugAndPlayPath = plugAndPlayPath;
}

void MinionSide::Init()
{
    m_memFile.open(m_memoryFilePath, std::ios::in | std::ios::out);
 
    m_masterProxy = std::make_shared<MasterProxy>(m_minionIp, m_minionPort);
    std::vector<std::tuple<int, Reactor<int>::Mode, std::shared_ptr<IInputProxy<int, int>>>> fdCallbacks;
    fdCallbacks.emplace_back(m_masterProxy->GetMasterProxyFd(), Reactor<int>::READ, m_masterProxy);
    fdCallbacks.emplace_back(fileno(stdin), Reactor<int>::READ, m_shutdownProxy);

    std::vector<std::pair<int, std::function<std::shared_ptr<ICommand<int>>()> >> creators;
    creators.emplace_back(READ_OPERATION_CREATOR,  ReadOperation::Creator);
    creators.emplace_back(WRITE_OPERATION_CREATOR, WriteOperation::Creator);

    m_framework = std::make_shared<Framework<int, int>>(fdCallbacks, creators, m_plugAndPlayPath, m_listener, nullptr);
}

std::fstream* MinionSide::GetMemFile()
{
    return &m_memFile;
}

void MinionSide::Run()
{
    Init();

    m_framework->Run();
}

void MinionSide::Stop()
{
    m_framework->Stop();
}

std::shared_ptr<ICommandData<int>> MinionSide::ShutdownProxy::GetCommandData(int fd, Reactor<int>::Mode mode)
{
    (void)mode; // unused

    int bytesRead = 0;

    char buffer[MAX_INPUT] = {0};

    bytesRead = read(fd, buffer, MAX_INPUT);
    assert(bytesRead > 0);

    if('q' == buffer[0])
    {
        Singleton<MinionSide>::GetInstance()->Stop();
    }    

    return nullptr;
}

} // namespace ilrd





