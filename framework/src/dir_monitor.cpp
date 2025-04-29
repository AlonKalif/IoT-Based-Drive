/*
    Developer: Alon Kalif
    File:      dir_monitor.cpp
    Reviewer:  
    Date: 	   22.12.2024
    Status:    In progress
    Version:   1.3 minor fixes
*/

#include <filesystem>
#include <memory>                   // For std::shared_ptr

#include <unistd.h>                 // For read()
#include <fcntl.h>                  // To check if fd is still open
#include <sys/inotify.h>            // For file monitoring & event type macros
#include <limits.h>                 // For NAME_MAX

#include "dir_monitor.hpp"			// For Class definition
#include "logger.hpp"               // For documentation

namespace ilrd
{

extern Logger* g_logger;

typedef std::pair<std::string, DirMonitor::EventType> eventData_t;

/* ============================ Class DirMonitor ============================ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

DirMonitor::DirMonitor(const std::string& dirPath): m_watcher(dirPath)
{
    ReadFolder(dirPath);

    m_typeToDisp = {
                    {DM_CREATE, &m_createDisp},
                    {DM_DELETE, &m_deleteDisp},
                    {DM_MODIFY, &m_modifyDisp}
                  };

    m_thread = std::thread(&DirMonitor::EventLoop, this);

    #ifndef NDEBUG
        g_logger->Log(DIR_MONITOR_MSG::FOLDER + m_dirPath);
    #endif
}

DirMonitor::~DirMonitor() noexcept
{
    try
    {
        m_watcher.CloseWatch();         
    }
    catch(const std::runtime_error& e)
    {
        g_logger->Log(DIR_MONITOR_MSG::DIR_MONITOR_DESTRUCTION_ERROR);
    }
       
    
    if(m_thread.joinable())
    {
        m_thread.join();
    }
}

/* ---------------------------- Member Functions ---------------------------- */

void DirMonitor::Register(ICallback<std::string>* callback, EventType listenTo)
{
    if(nullptr == callback)
    {
        g_logger->Log(DIR_MONITOR_MSG::REGISTER_ERROR, __FILE__, __LINE__);
        return;
    }

    m_typeToDisp[listenTo]->Attach(callback);

    #ifndef NDEBUG
        g_logger->Log(DIR_MONITOR_MSG::REGISTER);
    #endif
}

void DirMonitor::UnRegister(ICallback<std::string>* callback)
{
    if(nullptr == callback)
    {
        g_logger->Log(DIR_MONITOR_MSG::UNREGISTER_ERROR, __FILE__, __LINE__);

        return;
    }
    
    callback->Unsubscribe();

    #ifndef NDEBUG
        g_logger->Log(DIR_MONITOR_MSG::UNREGISTER);
    #endif
}

std::unordered_set<std::string> DirMonitor::GetExistingFiles()
{
    std::scoped_lock<std::mutex> lock(m_lock);
    return m_filesInFolder;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void DirMonitor::EventLoop()
{
    while(m_watcher.IsWatching())
    {
        HandleEvent();

        try
        {
            m_watcher.WaitForEvent(m_eventQueue);
        }
        catch(const std::runtime_error& e)
        {
            g_logger->Log(DIR_MONITOR_MSG::EVENT_LOOP_ERROR, __FILE__, __LINE__);
        }
    }

    #ifndef NDEBUG
        g_logger->Log(DIR_MONITOR_MSG::FOLDER_DONE + m_dirPath);
    #endif   
}

void DirMonitor::HandleEvent()
{
    while(!m_eventQueue.empty())
    {
        if(DM_CREATE == m_eventQueue.front().second)
        {
            std::scoped_lock<std::mutex> lock(m_lock);
            m_filesInFolder.emplace(m_eventQueue.front().first);
        }
        else if(m_filesInFolder.contains(m_eventQueue.front().first))
        {
            std::scoped_lock<std::mutex> lock(m_lock);
            m_filesInFolder.erase(m_eventQueue.front().first);
        }

        m_typeToDisp[m_eventQueue.front().second]->Notify(&m_eventQueue.front().first);
        m_eventQueue.pop();
    }
}

void DirMonitor::ReadFolder(const std::string& dirPath)
{
    for(auto const& entry : std::filesystem::directory_iterator{dirPath})
    {
        m_filesInFolder.emplace(entry.path().generic_string());
    }
}

/* ========================================================================== */





/* ======================= Nested Class InotifyWrapper ====================== */

const int    DirMonitor::InotifyWrapper::ERROR      = -1;
const size_t DirMonitor::InotifyWrapper::EVENT_SIZE = sizeof(struct inotify_event) + NAME_MAX + 1;
const size_t DirMonitor::InotifyWrapper::BUFF_SIZE  = EVENT_SIZE * 10;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ---------------------------------- ctor ---------------------------------- */

DirMonitor::InotifyWrapper::InotifyWrapper(const std::string& dirPath):
m_dirPath(dirPath), m_isWatching(true), m_folderFd(inotify_init1(IN_CLOEXEC)),
m_watcherFd(inotify_add_watch(m_folderFd, dirPath.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM)),
m_maskToType({{IN_CREATE, DM_CREATE}, {IN_MOVED_TO, DM_CREATE}, {IN_DELETE, DM_DELETE}, {IN_MOVED_FROM, DM_DELETE}, {IN_MODIFY, DM_MODIFY}})
{
    if(ERROR == m_folderFd || ERROR == m_watcherFd)
    {
        throw std::runtime_error("Inotify failed to initialize.");
    }
}

/* ---------------------------- Member Functions ---------------------------- */

void DirMonitor::InotifyWrapper::CloseWatch()
{
    if(ERROR == inotify_rm_watch(m_folderFd, m_watcherFd) ||
       ERROR == close(m_folderFd))
    {
        throw std::runtime_error("Inotify failed to close.");
    }
}

void DirMonitor::InotifyWrapper::WaitForEvent(std::queue<eventData_t>& dataOut)
{
    char buffer[BUFF_SIZE] = {0};
    ssize_t numBytes = read(m_folderFd, &buffer, BUFF_SIZE);
    
    if(ERROR == numBytes)
    {
        throw std::runtime_error("Inotify failed to read event.");
    }

    RecordData(buffer, numBytes, dataOut);
}

bool DirMonitor::InotifyWrapper::IsWatching()
{
    return m_isWatching;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void DirMonitor::InotifyWrapper::RecordData(char* buffer, ssize_t bytesRead, std::queue<eventData_t>& dataOut)
{   
    m_isWatching = (IN_IGNORED != reinterpret_cast<struct inotify_event*>(buffer)->mask);    
    
    for(char* runner = buffer; m_isWatching && (runner < buffer + bytesRead);)
    {
        struct inotify_event* eventData = reinterpret_cast<struct inotify_event*>(runner);

        dataOut.push(PrepareData(eventData->name, eventData->mask));

        runner += (sizeof(struct inotify_event) + eventData->len);        
    }
}

eventData_t DirMonitor::InotifyWrapper::PrepareData(const char* fileName, uint32_t mask)
{
    return std::make_pair(m_dirPath + "/" + std::string(fileName), m_maskToType[mask]);
}

/* ========================================================================== */

} // namespace ilrd





