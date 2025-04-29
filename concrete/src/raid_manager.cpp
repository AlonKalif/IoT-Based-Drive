/*
    Developer: Alon Kalif
    File:      raid_manager.cpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "raid_manager.hpp"
#include "logger.hpp"               // For documentation
#include "concrete_messages.hpp"    // Messages for logger 

namespace ilrd
{

extern Logger* g_logger;

std::pair<RaidManager::MinionAccessPoint, RaidManager::MinionAccessPoint>
RaidManager::OffsetToProxy(size_t offset)
{
    if(offset >= m_primaryMemSize * m_numMinions)
    {
        g_logger->Log(RAID_MSG::OFFSET_ERROR, __FILE__, __LINE__);
        throw std::invalid_argument(RAID_MSG::OFFSET_ERROR);
    }

    size_t primaryIndex  = CalcPrimaryIndex  (offset);
    size_t primaryOffset = CalcPrimaryOffset (offset, primaryIndex);

    size_t backupIndex  = CalcBackupIndex  (primaryIndex);
    size_t backupOffset = CalcBackupOffset (primaryOffset);

    assert(primaryIndex < 3);
    assert(backupIndex < 3);
    assert(primaryOffset < 4194304);
    assert((backupOffset >= 4194304) && (backupOffset < 8388608));

    return  std::make_pair< MinionAccessPoint, MinionAccessPoint >(
                MinionAccessPoint(m_minionProxies[primaryIndex], primaryOffset),
                MinionAccessPoint(m_minionProxies[backupIndex], backupOffset)
    );
}

void RaidManager::Config(const nlohmann::json& config, const std::string& masterIp)
{

    m_numMinions    = config.at("number_of_minions").get<size_t>();
    m_numBackups    = config.at("number_of_backups").get<size_t>();
    m_minionMemSize = config.at("minion_memory_size_in_bytes").get<size_t>();

    m_addresses.emplace_back(config.at("minion_1_ip").get<std::string>(),
                            config.at("minion_1_port").get<std::string>(),
                            masterIp,
                            config.at("proxy_1_port").get<std::string>());

    m_addresses.emplace_back(config.at("minion_2_ip").get<std::string>(),
                            config.at("minion_2_port").get<std::string>(),
                            masterIp,
                            config.at("proxy_2_port").get<std::string>());

    m_addresses.emplace_back(config.at("minion_3_ip").get<std::string>(),
                            config.at("minion_3_port").get<std::string>(),
                            masterIp,
                            config.at("proxy_3_port").get<std::string>());

    
    InitMinionProxies();
}

void RaidManager::InitMinionProxies()
{
    m_primaryMemSize = m_minionMemSize / 2;

    for(size_t i = 0; i < m_numMinions; ++i)
    {
        proxyAddresses addrs = m_addresses[i];

        try
        {
            m_minionProxies.emplace_back(std::make_shared<MinionProxy>(std::get<0>(addrs), std::get<1>(addrs), std::get<2>(addrs), std::get<3>(addrs)));
        }
        catch(const std::exception& e)
        {
            g_logger->Log(RAID_MSG::PROXY_INIT_ERROR + "minion's IP: " + std::get<0>(addrs) +
                          "minion proxy's port: " + std::get<3>(addrs), __FILE__, __LINE__);

            throw std::runtime_error(RAID_MSG::PROXY_INIT_ERROR + "minion's IP: " + std::get<0>(addrs) +
                          "minion proxy's port: " + std::get<3>(addrs)); // handle with throw?
        }
    }
}

inline size_t RaidManager::CalcPrimaryIndex(size_t offset)
{
    return static_cast<size_t>(offset / m_primaryMemSize);
}

inline size_t RaidManager::CalcPrimaryOffset(size_t offset, size_t primaryIndex)
{
    return offset - m_primaryMemSize * primaryIndex;
}

inline size_t RaidManager::CalcBackupIndex(size_t primaryIndex)
{
    return (primaryIndex + 1) % m_numMinions;
}

inline size_t RaidManager::CalcBackupOffset(size_t primaryOffset)
{
    return primaryOffset + m_primaryMemSize;
}

std::vector<std::shared_ptr<MinionProxy>>* RaidManager::GetMinionProxies()
{
    return &m_minionProxies;
}

} // namespace ilrd





