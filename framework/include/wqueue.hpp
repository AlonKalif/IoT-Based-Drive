/*
    Developer: Alon Kalif
    File:      WQueue.hpp
    Reviewer:  Shira
    Date: 	   03.12.2024
    Status:    Tested
    Version:   1.0
*/
#ifndef __ILRD_RD161_WQueue_HPP__
#define __ILRD_RD161_WQueue_HPP__

#include <chrono>             // For duration
#include <queue>              
#include <mutex>              
#include <condition_variable> 

namespace ilrd
{

enum WQueueStatus
{
    SUCCESS,
    TIME_OUT_EMPTY,
    TIME_OUT_LOCK
};

/*
    Container must implement the following functions:

    - front
    - push_back
    - pop_front
    - empty
*/
template<typename T, class Container = std::deque<T>>
class WQueue
{
public:
    
    WQueue  ()              = default;
    WQueue  (const WQueue&) = delete;
   ~WQueue  ()              = default;

    WQueue& operator= (const WQueue&) = delete;

    void         Pop     (T* out_p);     
    WQueueStatus Pop     (T* out_p, const std::chrono::milliseconds& timeToWait);
    void         Push    (const T& val); 
    const T&           Front    (); 
    bool         IsEmpty () const; 
private:
    Container m_container;
    mutable std::timed_mutex m_containerLock;
    std::condition_variable_any m_waitForElement;
};

template<typename T, class Container>
const T& WQueue<T, Container>::Front()
{
    std::unique_lock<std::timed_mutex> lock(m_containerLock);
    return m_container.front();
}

template<typename T, class Container>
bool WQueue<T, Container>::IsEmpty() const
{
    std::unique_lock<std::timed_mutex> lock(m_containerLock);
    return m_container.empty();
}

template<typename T, class Container>
void WQueue<T, Container>::Pop(T* out_p)
{
    if (!out_p) 
    {
        throw std::invalid_argument("Output pointer cannot be null");
    }

    std::unique_lock<std::timed_mutex> lock(m_containerLock);

    while(true == m_container.empty())
    {
        m_waitForElement.wait(lock);
    }
    
    *out_p = m_container.front();
    m_container.pop_front();
}

template<typename T, class Container>
WQueueStatus WQueue<T, Container>::Pop(T* out_p, const std::chrono::milliseconds& timeToWait)
{   
    std::chrono::time_point<std::chrono::steady_clock> timeout = 
                                std::chrono::steady_clock::now() + (timeToWait);

    if(!m_containerLock.try_lock_until(timeout))
    {
        return TIME_OUT_LOCK;
    }

    std::unique_lock<std::timed_mutex> lock(m_containerLock, std::adopt_lock_t());

    if(!m_waitForElement.wait_until(lock, timeout, [this](){return !m_container.empty();}))
    {
        return TIME_OUT_EMPTY;    
    }

    *out_p = m_container.front();
    m_container.pop_front();      
    
    return SUCCESS;
}

template<typename T, class Container>
void WQueue<T, Container>::Push(const T& val)
{
    {
        std::unique_lock<std::timed_mutex> lock(m_containerLock);
        m_container.push_back(val);
    }

    m_waitForElement.notify_one();
}

} // namespace ilrd

#endif //__ILRD_RD161_WQueue_HPP__