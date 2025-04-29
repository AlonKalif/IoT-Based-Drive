/*
    Developer: Alon Kalif
    File:      dir_monitor.hpp
    Reviewer:  
    Date: 	   22.12.2024
    Status:    In progress
    Version:   1.3 minor fixes
*/
#ifndef __ILRD_RD161_DIR_MONITOR_HPP__
#define __ILRD_RD161_DIR_MONITOR_HPP__

#include <string>
#include <thread> 
#include <unordered_set>

#include "dispatcher.hpp"

namespace ilrd
{
class DirMonitor
{
public:
    explicit    DirMonitor (const std::string& dirPath); // may throw runtime_error
               ~DirMonitor () noexcept;

                DirMonitor (const DirMonitor& other) = delete;
    DirMonitor& operator=  (const DirMonitor& other) = delete;
    
    enum EventType
    {
        DM_CREATE,
        DM_DELETE,
        DM_MODIFY
    };

    void Register   (ICallback<std::string>* callback, EventType listenTo);
    void UnRegister (ICallback<std::string>* callback);

    std::unordered_set<std::string> GetExistingFiles(); 

private:

    typedef std::pair<std::string, DirMonitor::EventType> eventData_t;

    class InotifyWrapper
    {
    public:

        InotifyWrapper(const std::string& dirPath);   // may throw runtime_error
        void CloseWatch();                            // may throw runtime_error
        bool IsWatching();
        void WaitForEvent(std::queue<eventData_t>& dataOut);      // may throw runtime_error
        
    private:
        std::string m_dirPath;
        bool m_isWatching;
        int m_folderFd;
        int m_watcherFd;
        std::unordered_map<uint32_t, EventType> m_maskToType;

        static const int ERROR;
        static const size_t BUFF_SIZE;
        static const size_t EVENT_SIZE;

        void RecordData(char* buffer, ssize_t bytesRead, std::queue<eventData_t>& dataOut);
        eventData_t PrepareData(const char* fileName, uint32_t mask);
    };
    
    InotifyWrapper          m_watcher;
    std::queue<eventData_t> m_eventQueue;
    std::mutex              m_lock;

    Dispatcher<std::string> m_createDisp;
    Dispatcher<std::string> m_deleteDisp;
    Dispatcher<std::string> m_modifyDisp;

    std::string m_dirPath;
    std::unordered_set<std::string> m_filesInFolder;
    std::unordered_map< EventType, Dispatcher<std::string >*> m_typeToDisp;
    std::thread m_thread;


    void ReadFolder (const std::string& dirPath);
    void EventLoop ();
    void HandleEvent();
};

} // namespace ilrd

#endif // __ILRD_RD161_DIR_MONITOR_HPP__
