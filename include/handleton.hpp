/*
    Developer: Alon Kalif
    File:      handleton.hpp
    Reviewer:  
    Date: 	   16.12.2024
    Status:    tested
    Version:   1.2 
*/
#ifndef ILRD_RD161_HANDLETON_HPP
#define ILRD_RD161_HANDLETON_HPP

#ifdef __HANDLETON__

#include <mutex>      // for std::mutex 
#include <stdexcept>
#include <atomic>
#include <cstdlib>    // for std::atexit

#endif // __HANDLETON__

namespace ilrd
{
    
template <class T, class ...Args>
class Singleton
{
public:
               Singleton ()                        = delete;
              ~Singleton ()                        = delete;
               Singleton (      Singleton&& other) = delete;
               Singleton (const Singleton&  other) = delete;
    Singleton& operator= (const Singleton&  other) = delete;

    static T* GetInstance(Args... args);
    
private:
    #ifdef __HANDLETON__

    static std::atomic<T*> m_instance;
    static std::mutex m_mutex;
    static void CleanUp();

    #endif // __HANDLETON__
};

#ifdef __HANDLETON__

template <class T, class ...Args>
std::mutex Singleton<T, Args...>::m_mutex;

template <class T, class ...Args>
std::atomic<T*> Singleton<T, Args...>::m_instance = nullptr;

template <class T, class ...Args>
T* Singleton<T, Args...>::GetInstance(Args... args)
{
    T* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);

    if(nullptr == tmp)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        tmp = m_instance.load(std::memory_order_relaxed);

        if(nullptr == tmp)
        {
            tmp = new T(args...);
            if(0 != std::atexit(Singleton<T, Args...>::CleanUp))
            {
                delete tmp;
                throw std::runtime_error ("at exit registration failed");
            }
            
            std::atomic_thread_fence(std::memory_order_release);
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}

template <class T, class ...Args>
void Singleton<T, Args...>::CleanUp()
{
    delete m_instance;
}

#endif // __HANDLETON__

} // namespace ilrd

#endif	// ILRD_RD161_HANDLETON_HPP 
