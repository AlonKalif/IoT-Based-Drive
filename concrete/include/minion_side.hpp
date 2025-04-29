/*
    Developer: Alon Kalif
    File:      minion_side.hpp
    Reviewer:  
    Date: 	   29.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_MINION_SIDE_HPP
#define ILRD_RD161_MINION_SIDE_HPP

#include <fstream>             // For file handling

#include <nlohmann/json.hpp>   // To handle json config file

#define __HANDLETON__
#include "handleton.hpp"       // To make MinionSide singleton
#undef __HANDLETON__

#include "select_listener.hpp" // Reactor listener
#include "master_proxy.hpp"            
#include "framework.hpp"   

namespace ilrd
{

class MinionSide
{
public:
    ~MinionSide();

    void Run(); // Blocking
    void Stop();
    
    std::fstream* GetMemFile();

    void Config(const std::string& ip, const std::string& port,
                const std::string& file, const std::string& plugAndPlayPath);

private:
    MinionSide();
    friend class Singleton<MinionSide>;

    class ShutdownProxy : public IInputProxy<int, int>
    {
    public:
        std::shared_ptr<ICommandData<int>> GetCommandData(int fd, Reactor<int>::Mode mode) override;
    };

    std::string  m_minionIp;
    std::string  m_minionPort;
    std::string  m_memoryFilePath;
    std::string  m_plugAndPlayPath;
    std::fstream m_memFile;

    std::shared_ptr<Reactor<int>::Ilistener> m_listener;
    std::shared_ptr<MasterProxy>             m_masterProxy;
    std::shared_ptr<Framework<int, int>>     m_framework;
    std::shared_ptr<ShutdownProxy>           m_shutdownProxy;

    void Init();
};


} // namespace ilrd

#endif	// ILRD_RD161_MINION_SIDE_HPP 
