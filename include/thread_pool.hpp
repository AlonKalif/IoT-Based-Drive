/*
    Developer: Alon Kalif
    File:      thread_pool.hpp
    Reviewer:  
    Date: 	   00.00.2024
    Status:    In progress
    Version:   1.0
*/
#ifndef ILRD_RD161_THREAD_POOL_HPP
#define ILRD_RD161_THREAD_POOL_HPP


#include <memory>           // For std::shared_ptr
#include <thread>           // For hardware_concurrency(), threads
#include <future>           // To implement future tasks
#include <unordered_map>

#include "handleton.hpp"    // To make ThreadPool singleton
#include "logger.hpp"
#include "wqueue.hpp"       // To store tasks
#include "pq_wrapper.hpp"   // Container of wqueue
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

class ThreadPool
{
public:
               ~ThreadPool ();
                ThreadPool (const ThreadPool&) = delete;
    ThreadPool& operator=  (const ThreadPool&) = delete;
    
    class ITPTask
    {
    public:
                 ITPTask ()               = default;
        virtual ~ITPTask ()               = default;
                 ITPTask (const ITPTask&) = default;

        virtual void operator()() = 0;
    };
    
    enum Priority {LOW, MID, HIGH};

    void Add          (std::shared_ptr<ITPTask> task, enum Priority priority = LOW);
    void Resume       ();
    void Pause        (); 
    void Stop         (); 
    void SetNumThread (size_t numOfThreads); // may throw system_error

    static const size_t DEFAULT_NUM_THREADS = 8;

    template< class F, class... Args >
    class FutureTask : public ITPTask
    {
    public:
        FutureTask(std::function<F(Args...)> func, Args... args);
        FutureTask(const FutureTask&) = default;

        virtual     ~FutureTask() = default;
        virtual void operator()();

        F GetResult();
        
    private:
        std::packaged_task<F(Args...)> m_futureTask;
        std::tuple<Args...> m_args;
        std::future<F> m_result;
    };


private:
    explicit ThreadPool (size_t numOfThreads = std::thread::hardware_concurrency()); // may throw bad_alloc, system_error. num_of_threads is recommendation only 
    friend class Singleton<ThreadPool>;

    class WorkerThread;
    class AdminTask;

    typedef std::pair<Priority, std::shared_ptr<ITPTask>> PrioritizedTask;

    std::unordered_map<std::thread::id, std::shared_ptr<WorkerThread>> m_pool;

    WQueue<PrioritizedTask, PQueue<PrioritizedTask>> m_waitingPQ;
    
    size_t                      m_numThreads;
    std::mutex                  m_poolGuard;
    std::mutex                  m_pauseMutex;
    std::condition_variable_any m_pauseCV;
    bool                        m_blockNewTasks;
    std::atomic<bool>           m_isPaused;

    void AddThreads    (size_t newNumThreads);
    void RemoveThreads (size_t newNumThreads);
};

/* =========================== FutureTask - ITPTask =========================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

template< class F, class... Args>
ThreadPool::FutureTask<F, Args...>::FutureTask(std::function<F(Args...)> func, Args... args)
: m_futureTask(std::packaged_task(func)), m_args(args...), m_result(m_futureTask.get_future())
{

}

template< class F, class... Args>
void ThreadPool::FutureTask<F, Args...>::operator()()
{
    #ifndef NDEBUG
        g_logger->Log(TP_MSG::FUTURE_OP);
    #endif

    std::apply(m_futureTask, m_args);
}

template< class F, class... Args>
F ThreadPool::FutureTask<F, Args...>::GetResult()
{
    #ifndef NDEBUG
        g_logger->Log(TP_MSG::FUTURE_DONE);
    #endif
    
    return m_result.get();
}

} // namespace ilrd




#endif	// ILRD_RD161_THREAD_POOL_HPP 
