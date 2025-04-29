/*
    Developer: Alon Kalif
    File:      logger.hpp
    Reviewer:  
    Date: 	   05.01.2024
    Status:    In progress
    Version:   1.1 - Log overload
*/
#ifndef __ILRD_RD161_LOGGER_HPP__
#define __ILRD_RD161_LOGGER_HPP__

#include <string>
#include <thread>
#include <unordered_map>
#include <fstream>          // std::ofstream

#include "wqueue.hpp"       // For thread-safe waiting queue
#include "handleton.hpp"    // For making the class singleton

namespace ilrd
{

class Logger
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    ~Logger() noexcept;
    
    void Log(const std::string& msg, const std::string& file, int line) noexcept;
    void Log(const std::string& msg, const std::string& file) noexcept;
    void Log(const std::string& msg) noexcept;

    void Config(const std::string& logFilePath);

    
private:
    Logger (); // may throw invalid_argument
    friend class Singleton<Logger>;

    WQueue<std::string> m_msgQueue;
    bool                m_isRunning;
    std::thread         m_thread;
    std::ofstream       m_logFile;

    void        LogLoop   ();
    std::string CreateMsg (std::string msg, std::string file, int line);

};


} // namespace ilrd
#endif // __ILRD_RD161_LOGGER_HPP__
