/*
    Developer: Alon Kalif
    File:      raid_manager.hpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_RAID_MANAGER_HPP
#define ILRD_RD161_RAID_MANAGER_HPP

#include <memory>   // For shared_ptr
#include <vector>
#include <tuple>

#include <nlohmann/json.hpp>	// To handle json config file

#define __HANDLETON__
#include "handleton.hpp"        // To make RaidManager singleton
#undef __HANDLETON__
#include "minion_proxy.hpp"     // To direct requests to relevant proxy

namespace ilrd
{

class RaidManager
{
public:

    // An access point specifies the relevant minion proxy and the offset in it
    using MinionAccessPoint = std::pair<std::shared_ptr<MinionProxy>, size_t>;

    std::pair<MinionAccessPoint, MinionAccessPoint> OffsetToProxy(size_t offset);  // may throw invalid_argument

    void Config(const nlohmann::json& config, const std::string& masterIp);

    std::vector<std::shared_ptr<MinionProxy>>* GetMinionProxies();

private:
    RaidManager() = default; 
    friend class Singleton<RaidManager>;
    
    size_t m_numMinions;      
    size_t m_minionMemSize;   
    size_t m_numBackups;      
    size_t m_primaryMemSize;

    using proxyAddresses = std::tuple<std::string, std::string, std::string, std::string>;

    std::vector<std::shared_ptr<MinionProxy>> m_minionProxies;
    std::vector<proxyAddresses> m_addresses;

    void InitMinionProxies();

    inline size_t CalcPrimaryIndex (size_t offset);
    inline size_t CalcPrimaryOffset(size_t offset, size_t primaryIndex);
    inline size_t CalcBackupIndex  (size_t primaryIndex);
    inline size_t CalcBackupOffset (size_t primaryOffset);
};

} // namespace ilrd

#endif	// ILRD_RD161_RAID_MANAGER_HPP 
