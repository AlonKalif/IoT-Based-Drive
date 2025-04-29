/*                                 ¯\_(ツ)_/¯                                 */
/*
    Developer: Alon Kalif
    File:      scheduler.cpp
    Reviewer:  Irenaaaaaaaa
    Date: 	   27.12.2024
    Status:    Ready for CR
    Version:   1.0
*/
#include <iostream>         // temp
#include <cstring>          // For memset
#include <cassert>			// For assert macro
#include <ctime>

#include "scheduler.hpp"	// For Class definition
#include "logger.hpp"       // For logging
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

/* ============================ Class Scheduler ============================= */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

Scheduler::Scheduler() : m_schedTimer(Timer(OnExpiration, this))
{
    // empty
}

Scheduler::~Scheduler() = default;

/* ---------------------------- member functions ---------------------------- */

void Scheduler::AddTask(std::shared_ptr<ISchedTask> task, const std::chrono::milliseconds& delta)
{
    if(nullptr == task)
    {
        g_logger->Log(SCHEDULER_MSG::ADD_ERROR, __FILE__, __LINE__);

        return;
    }

    if(m_pq.IsEmpty() || ((delta + GetCurrTime()) < m_pq.Front().second))
    {
        std::lock_guard<std::mutex> lock(m_lock);

        m_schedTimer.Set(delta);
    }

    m_pq.Push(std::make_pair(task, GetCurrTime() + delta));
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

void Scheduler::OnExpiration(union sigval thisSched)
{
    Scheduler* schedPtr = reinterpret_cast<Scheduler*>(thisSched.sival_ptr);
    Item currTask;

    // timedout:
    schedPtr->m_pq.Pop(&currTask);

    currTask.first->Execute(); 

    if(!schedPtr->m_pq.IsEmpty())
    {
        std::chrono::milliseconds newTimerVal = schedPtr->m_pq.Front().second - schedPtr->GetCurrTime();

        if(0 >= newTimerVal.count())
        {
            newTimerVal = std::chrono::milliseconds(1);
            // goto timedout;
        }

        std::lock_guard<std::mutex> lock(schedPtr->m_lock);

        schedPtr->m_schedTimer.Set(newTimerVal);
    }
}

std::chrono::milliseconds Scheduler::GetCurrTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::system_clock::now().time_since_epoch()
            );
}

/* ~~~~~~~~~~~~~~~~~~~~~~~ Priority queue Comparator ~~~~~~~~~~~~~~~~~~~~~~~~ */

bool Scheduler::Comparator::operator()(const Item& a, const Item& b)
{
    // std::cout << "Comparator\n";

    if (a.second > b.second)
    {
        return true;
    }
    return false;
}

/* ========================================================================== */
/* 
                                     
                                                                              */
/* =========================== Nested Class Timer =========================== */

const int    Scheduler::Timer::ERROR = -1;
const size_t Scheduler::Timer::MEGA  = 1000000; 
const size_t Scheduler::Timer::KILO  = 1000; 

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

Scheduler::Timer::Timer(void (*onExpiration)(union sigval), Scheduler* schedPtr)
{
    struct sigevent config;
    memset(&config, 0, sizeof(struct sigevent));

    config.sigev_notify          = SIGEV_THREAD;
    config.sigev_notify_function = onExpiration;
    config.sigev_value.sival_ptr = schedPtr;
    config.sigev_signo           = SIGALRM;

    if(ERROR == timer_create(CLOCK_REALTIME, &config, &m_timerid))
    {
        throw std::runtime_error("failed to create system timer");
    }
}

Scheduler::Timer::~Timer()
{
    if(ERROR == timer_delete(m_timerid))
    {
        g_logger->Log("timer destruction failed.", __FILE__, __LINE__);
    }
}

/* ---------------------------- member functions ---------------------------- */

void Scheduler::Timer::Set(std::chrono::milliseconds value)
{
    m_nextExpiration.it_value.tv_nsec = MiliToNano (value);
    m_nextExpiration.it_value.tv_sec  = MiliToSec  (value);

    if(ERROR == timer_settime(m_timerid, 0, &m_nextExpiration, NULL))
    {
        throw std::runtime_error("failed to set system timer");
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

size_t Scheduler::Timer::MiliToNano(std::chrono::milliseconds value)
{
    return (value.count() % KILO) * MEGA;
}

size_t Scheduler::Timer::MiliToSec(std::chrono::milliseconds value)
{
    return value.count() / KILO;
}

/* ========================================================================== */

} // namespace ilrd





