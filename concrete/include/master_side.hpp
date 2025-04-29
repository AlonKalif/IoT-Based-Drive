/*
    Developer: Alon Kalif
    File:      master_side.hpp
    Reviewer:  
    Date: 	   11.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_MASTER_SIDE_HPP
#define ILRD_RD161_MASTER_SIDE_HPP

#include <thread>               // To run NBD client on a thread
#include <memory>               // For shared_ptr

#include <nlohmann/json.hpp>	// To handle json config file

#define __HANDLETON__
#include "handleton.hpp"        // To make MasterSide singleton
#undef __HANDLETON__

#include "nbd_proxy.hpp"            
#include "framework.hpp"         
#include "select_listener.hpp"  // Fd select listener for Reactor

namespace ilrd
{

class MasterSide
{
public:
    void Run();     // Blocking
    void Stop();
    
    void Config(const nlohmann::json& config);

    std::shared_ptr<NBDProxy> GetNbdProxy() const;

private:
    MasterSide();
    friend class Singleton<MasterSide>;

    std::string m_plugAndPlayPath;

    std::shared_ptr<NBDProxy>                m_nbdProxy;
    std::shared_ptr<Reactor<int>::Ilistener> m_listener;
    std::shared_ptr<Framework<int, int>>     m_framework;

    void InitFramework();
};

} // namespace ilrd

#endif	// ILRD_RD161_MASTER_SIDE_HPP 
