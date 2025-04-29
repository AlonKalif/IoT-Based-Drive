/*
    Developer: Alon Kalif
    File:      async_injection.cpp
    Reviewer:  
    Date: 	   29.12.2024
    Status:    In progress
    Version:   1.0
*/

#include "logger.hpp"
#include "async_injection.hpp"	// For Class definition
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;
extern Scheduler* g_scheduler;


/* ========================== Class AsyncInjection ========================== */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

AsyncInjection::AsyncInjection(std::function<bool()> action, std::chrono::milliseconds interval)
: m_action(action), m_interval(interval), m_invoker(std::make_shared<AsyncInvoker>(*this))
{
    #ifndef NDEBUG
        g_logger->Log(AI_MSG::AI_INIT);
    #endif
    
    g_scheduler->AddTask(m_invoker, m_interval);
}

/* ---------------------------- member functions ---------------------------- */

void AsyncInjection::Perform() const
{
    if(m_action())
    {
        delete this;

        return;
    }

    g_scheduler->AddTask(m_invoker, m_interval);
}

/* ========================================================================== */



/* ======================== Nested Class AsyncInvoker ======================= */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

AsyncInjection::AsyncInvoker::AsyncInvoker(const AsyncInjection& ai) : m_ai(ai)
{
    // MIL
}

/* ---------------------------- member functions ---------------------------- */

void AsyncInjection::AsyncInvoker::Execute()
{
    m_ai.Perform();
}

/* ========================================================================== */

} // namespace ilrd





