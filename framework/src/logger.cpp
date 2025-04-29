/*
    Developer: Alon Kalif
    File:      logger.cpp
    Reviewer:  
    Date: 	   05.01.2024
    Status:    In progress
    Version:   1.1 - Log overload
*/

#include <iostream>         // For I/O operations
#include <fstream>          // For file handling

#include <iomanip>          // For put_time

#include "logger.hpp"

namespace ilrd
{

void Logger::Config(const std::string& logFilePath)
{
    if (m_logFile.is_open()) 
    {
        m_logFile.close();
    }

    m_logFile.open(logFilePath, std::ios::out);
    if(!m_logFile.is_open())
    {
        throw std::invalid_argument("log file failed to open");
    }

    m_thread = std::thread(&Logger::LogLoop, this);
}

/* ------------------------------ ctors & dtor ------------------------------ */

Logger::Logger(): m_isRunning(true)
{
    
}

Logger::~Logger()
{
    m_isRunning = false;

    if(m_thread.joinable())
    {
        m_thread.join();
    }
    
    m_logFile.close();
}

/* ---------------------------- member functions ---------------------------- */

// std::string Logger::CreateMsg(std::string msg, std::string file, int line)
// {
//     auto t = std::time(nullptr);
//     auto tm = *std::localtime(&t);

//     std::ostringstream oss;
//     oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
//     auto timeStamp = oss.str();

//     std::string lineStr = (line != -1) ? std::to_string(line) : "";

//     return timeStamp + " " + msg + " " + file + " " + lineStr + "\n";
// }

std::string Logger::CreateMsg(std::string msg, std::string file, int line)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    char timeStamp[100];
    if (std::strftime(timeStamp, sizeof(timeStamp), "%d-%m-%Y %H:%M:%S", &tm) == 0)
    {
        // Fallback in case formatting fails.
        std::snprintf(timeStamp, sizeof(timeStamp), "unknown time");
    }

    std::string lineStr = (line != -1) ? std::to_string(line) : "";

    return std::string(timeStamp) + " " + msg + " " + file + " " + lineStr + "\n";
}

void Logger::Log(const std::string& msg) noexcept
{
    Log(msg, "");
}

void Logger::Log(const std::string& msg, const std::string& file) noexcept
{
    Log(msg, file, -1);
}

void Logger::Log(const std::string& msg, const std::string& file, int line) noexcept
{
    try
    {
        m_msgQueue.Push(CreateMsg(msg, file, line));
    }
    catch(const std::bad_alloc& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Logger::LogLoop()
{
    while(m_isRunning || !m_msgQueue.IsEmpty())
    {
        std::string msg;
        m_msgQueue.Pop(&msg, std::chrono::milliseconds(100));
        m_logFile << msg;
    }
}

} // namespace ilrd





