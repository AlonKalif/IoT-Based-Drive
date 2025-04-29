/*
    Developer: Alon Kalif
    File:      thread_pool.cpp
    Reviewer:  
    Date: 	   20.12.2024
    Status:    In progress
    Version:   1.1          // Added loggings, fixed future tasks
*/
#include <iostream>         // remove later

#include "thread_pool.hpp"	// For Class definition
#include "logger.hpp"       // For documentations
#include "handleton.hpp"    // For singleton threadpool getter

namespace ilrd
{

/* ====================== WorkerThread - thread wrapper ===================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

class ThreadPool::WorkerThread
{
public:
    WorkerThread(ThreadPool* threadPoolPtr);
    ~WorkerThread();
    void HandleTasks(ThreadPool* threadPoolPtr);

    void Join();
    void Stop();
    // void Run();

    std::thread::id GetId() const;

private:
    std::atomic<bool> m_isRunning {true}; // remove atomic later ??
    std::thread m_worker;
};

ThreadPool::WorkerThread::~WorkerThread()
{
    Join();
}

// ctor
ThreadPool::WorkerThread::WorkerThread(ThreadPool* threadPoolPtr)
: m_isRunning(true), m_worker(&WorkerThread::HandleTasks, this, threadPoolPtr)
{
}

// Join
void ThreadPool::WorkerThread::WorkerThread::Join()
{
    if (m_worker.joinable())
    {
        m_worker.join();
    }
}

// get ID
std::thread::id ThreadPool::WorkerThread::GetId() const
{
    return m_worker.get_id();
}

// Stop
void ThreadPool::WorkerThread::Stop()
{
    m_isRunning.store(false, std::memory_order_acquire);
}

// thread function
void ThreadPool::WorkerThread::HandleTasks(ThreadPool* poolPtr)
{
    #ifndef NDEBUG
        g_logger->Log(TP_MSG::THREAD_RUN);
    #endif
    
    thread_local PrioritizedTask currentTask; 

    while(m_isRunning)
    {
        while(true == poolPtr->m_isPaused)
        {
            std::unique_lock<std::mutex> lock(poolPtr->m_pauseMutex);
            poolPtr->m_pauseCV.wait(lock);
        }

        poolPtr->m_waitingPQ.Pop(&currentTask);
        
        (*std::get<1>(currentTask))();
    }

    #ifndef NDEBUG
        g_logger->Log(TP_MSG::THREAD_STOP);
    #endif
}

/* =========================== AdminTask - ITPTask =========================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

// AdminTask 
class ThreadPool::AdminTask : public ThreadPool::ITPTask
{
public:
    enum TaskType
    {
        STOP_THREAD,
        BLANK_TASK
    };

    enum AdminPriority 
    {
        ADMIN_LOWEST  = LOW - 1,
        ADMIN_LOW     = LOW,
        ADMIN_MID     = MID,
        ADMIN_HIGH    = HIGH,
        ADMIN_HIGHEST = HIGH + 1
    };

    AdminTask(TaskType task);
    void operator()();

    static void AddTasks(size_t amount, AdminPriority taskPriority, TaskType type);

private:
    TaskType m_task;
};

// Ctor
ThreadPool::AdminTask::AdminTask(TaskType task)
: m_task(task)
{
}

// operator ()
void ThreadPool::AdminTask::operator()()
{
    ThreadPool* poolPtr = Singleton<ThreadPool>::GetInstance();

    std::lock_guard<std::mutex> lock(poolPtr->m_poolGuard);
    auto thisWorker = poolPtr->m_pool.find(std::this_thread::get_id());

    if(thisWorker != poolPtr->m_pool.end())
    {

        switch (m_task)
        {
        case STOP_THREAD:

            thisWorker->second->Stop();
            #ifndef NDEBUG
                g_logger->Log(TP_MSG::ADMIN_STOP);
            #endif

        break;
        case BLANK_TASK:

            #ifndef NDEBUG
                g_logger->Log(TP_MSG::ADMIN_BLANK);
            #endif

            break;
        default:
            break;
        }
    }
}

// add tasks
void ThreadPool::AdminTask::AddTasks(size_t amount, AdminPriority taskPriority, TaskType taskType)
{
    ThreadPool* poolPtr = Singleton<ThreadPool>::GetInstance();

    std::shared_ptr<ITPTask> newTaskPtr = std::make_shared<AdminTask>(taskType);
    PrioritizedTask newTask = std::make_pair(static_cast<Priority>(taskPriority), newTaskPtr);
    for(size_t i = 0; i < amount; ++i)
    {
        // std::scoped_lock<std::mutex> lock(tp->m_poolGuard);
        poolPtr->m_waitingPQ.Push(newTask);// this can throw !!!!

        #ifndef NDEBUG
            g_logger->Log(TP_MSG::NEW_TASK);
        #endif
    }
}

/* =============================== ThreadPool =============================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------- Ctor & Dtor ------------------------------ */

ThreadPool::ThreadPool(size_t numOfThreads)
: m_numThreads(numOfThreads ? numOfThreads : DEFAULT_NUM_THREADS),
  m_blockNewTasks(false), m_isPaused(false)
{
    #ifndef NDEBUG
            g_logger->Log(TP_MSG::TP_INIT + std::to_string(m_numThreads));
    #endif

    AddThreads(m_numThreads);
}

ThreadPool::~ThreadPool()
{
    Resume();
    AdminTask::AddTasks(m_numThreads, AdminTask::ADMIN_HIGHEST, AdminTask::STOP_THREAD);

    for (auto& thread : m_pool)
    {
        thread.second->Join();
    }

    #ifndef NDEBUG
            g_logger->Log(TP_MSG::TP_DESTROYED);
    #endif
}

/* ----------------------------- Public Methods ----------------------------- */

void ThreadPool::SetNumThread(size_t newNumThreads)
{
    int diff = m_numThreads - newNumThreads;
    m_numThreads = newNumThreads;

    if(0 > diff)
    {
        size_t amountToAdd = -diff;
        AddThreads(amountToAdd);
    }
    else if(0 < diff)
    {
        size_t amountToRemove = diff;
        RemoveThreads(amountToRemove);
    }

    #ifndef NDEBUG
            g_logger->Log(TP_MSG::SET_NUM_THREADS + std::to_string(m_numThreads));
    #endif
}

void ThreadPool::Pause()
{
    m_isPaused.store(true);
    AdminTask::AddTasks(m_numThreads, AdminTask::ADMIN_HIGHEST, AdminTask::BLANK_TASK);
}

void ThreadPool::Resume()
{
    m_isPaused.store(false);
    m_pauseCV.notify_all();
}

void ThreadPool::Stop()
{
    m_blockNewTasks = true;
    AdminTask::AddTasks(m_numThreads, AdminTask::ADMIN_LOWEST, AdminTask::STOP_THREAD);
}

void ThreadPool::Add(std::shared_ptr<ITPTask> taskPtr, enum Priority priority)
{
    if(false == m_blockNewTasks)
    {
        PrioritizedTask newTask = std::make_pair(static_cast<Priority>(priority), taskPtr);

        m_waitingPQ.Push(newTask);
    }
}  

/* ----------------------------- Private Methods ---------------------------- */

void ThreadPool::AddThreads(size_t amountToAdd)
{
    std::unique_lock<std::mutex> lock(m_poolGuard);  

    for(size_t i = 0; i < amountToAdd; ++i)
    {        
        std::shared_ptr<WorkerThread> currentWorker = std::make_shared<WorkerThread>(this);
        m_pool.emplace(currentWorker->GetId(), currentWorker);
    }
}

void ThreadPool::RemoveThreads(size_t amountToRemove)
{
    AdminTask::AddTasks(amountToRemove, AdminTask::ADMIN_HIGHEST, AdminTask::STOP_THREAD);
}

} // namespace ilrd





