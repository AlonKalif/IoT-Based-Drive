/*
    Developer: Alon Kalif
    File:      scheduler.cpp
    Reviewer:  Irenaaaaaaaa
    Date: 	   27.12.2024
    Status:    Ready for CR
    Version:   1.0
*/
#ifndef __ILRD_SCHEDULER_HPP__
#define __ILRD_SCHEDULER_HPP__

#include <chrono>           // For time keeping
#include <memory>           // For std::shraed_pointer
#include <queue>            // For std::priority_queue
#include <functional>       // For std::function
#include <mutex>
#include <vector>

#include <signal.h>         // For struct sigevent

#include "wqueue.hpp"
#include "pq_wrapper.hpp"
#include "handleton.hpp"    // For making the class singleton

namespace ilrd
{

class Scheduler
{
public:
    
    class ISchedTask
    {
    public:
        virtual ~ISchedTask  () = default;
        virtual void Execute () = 0;
    };
    
    ~Scheduler();
    void AddTask(std::shared_ptr<ISchedTask> task, const std::chrono::milliseconds& delta);

private:
    Scheduler(); // for Singleton
    friend class Singleton<Scheduler>;
    
    using Item = std::pair<std::shared_ptr<ISchedTask>, std::chrono::milliseconds>;

    struct Comparator
    {
        bool operator()(const Item& a, const Item& b);
    };

    class Timer
    {
    public:
        Timer(void (*onExpiration)(union sigval), Scheduler* schedPtr);    // may throw runtime_error
       ~Timer() noexcept;   

        void Set(std::chrono::milliseconds value);                         // may throw runtime_error

    private:
        struct itimerspec m_nextExpiration;
        timer_t m_timerid;

        static const size_t MEGA; 
        static const size_t KILO; 
        static const int ERROR;

        size_t MiliToNano (std::chrono::milliseconds value);
        size_t MiliToSec  (std::chrono::milliseconds value);
    };

    std::mutex m_lock;
    // std::priority_queue< Item, std::vector<Item>, Comparator > m_pq;
    WQueue<Item, PQueue<Item, std::vector<Item>, Comparator>> m_pq; 
    Timer m_schedTimer;

    static void OnExpiration(union sigval thisSched);
    std::chrono::milliseconds GetCurrTime();
};

} // namespace ilrd

#endif //__ILRD_SCHEDULER_HPP__
