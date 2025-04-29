/*
    Developer: Alon Kalif
    File:      async_injection.hpp
    Reviewer:  
    Date: 	   29.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_ASYNC_INJECTION_HPP
#define ILRD_RD161_ASYNC_INJECTION_HPP

#include "scheduler.hpp"    // For ISchedTask

namespace ilrd
{

class AsyncInjection
{
public:
    AsyncInjection(std::function<bool()> action, std::chrono::milliseconds interval);
    void Perform() const;

private:
    ~AsyncInjection() = default; // To prevent user from deleting

    class AsyncInvoker : public Scheduler::ISchedTask
    {
    public:
        AsyncInvoker(const AsyncInjection& ai);
        void Execute() override;
    private:
        const AsyncInjection& m_ai;
    };

    std::function<bool()>         m_action;
    std::chrono::milliseconds     m_interval;
    std::shared_ptr<AsyncInvoker> m_invoker;
};


} // namespace ilrd

#endif	// ILRD_RD161_ASYNC_INJECTION_HPP 
