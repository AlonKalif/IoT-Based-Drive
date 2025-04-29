/*
    Developer: Alon Kalif
    File:      dispatcher.hpp
    Reviewer:  
    Date: 	   00.00.2024
    Status:    In progress
    Version:   1.0
*/
#ifndef __ILRD_RD161_DISPATCHER_HPP__
#define __ILRD_RD161_DISPATCHER_HPP__

#include <iostream>     // remove later

#include <unordered_set>
#include <iterator> // For std::next

#include "message_manager.hpp"
#include "handleton.hpp"
#include "logger.hpp"

namespace ilrd
{

extern Logger* g_logger;

template <class T>
class ICallback;

template <class T>
class Dispatcher
{
public:
    explicit Dispatcher() = default;
    Dispatcher(const Dispatcher&) = delete; 
    Dispatcher& operator=(const Dispatcher&) = delete; 
    ~Dispatcher() noexcept;
    void Attach(const ICallback<T>* callback);
    void Notify(const T*);
    
private:
    friend class ICallback<T>;
    std::unordered_set<ICallback<T>*> m_callbacks;
    void Detach(ICallback<T>* callback);
};

template <class T>
class ICallback
{
public:
    ICallback() = default;
    void Unsubscribe();
    virtual void Update(const T*) = 0;
    virtual ~ICallback();

private:
    friend class Dispatcher<T>;
    Dispatcher<T>* m_dispatcher;

    void Subscribe(Dispatcher<T>* disp);
    void NotifyDeath();
    virtual void OnNotifyDeath();
};


// static Logger* g_logger = Singleton<Logger>::GetInstance();

/* ======================== Dispatcher Implementation ======================= */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ---------------------------------- dtor ---------------------------------- */

template <class T>
Dispatcher<T>::~Dispatcher() noexcept
{
    std::unordered_set<ICallback<T>*> tmp(m_callbacks);

    for(auto subscriber : tmp)
    {
        subscriber->NotifyDeath();
    }
}

/* ---------------------------- member functions ---------------------------- */

template <class T>
void Dispatcher<T>::Attach(const ICallback<T>* callback)
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::ATTACH);
    #endif

    ICallback<T>* call = const_cast<ICallback<T>*>(callback);
    m_callbacks.insert(call);

    call->Subscribe(this);
}

template <class T>
void Dispatcher<T>::Notify(const T* data)
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::NOTIFY);
    #endif

    std::unordered_set<ICallback<T>*> tmp(m_callbacks);

    for(auto subscriber : tmp)
    {
        subscriber->Update(data);
    }
}

template <class T>
void Dispatcher<T>::Detach(ICallback<T>* callback)
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::DETACH);
    #endif
    m_callbacks.erase(callback); 
}

/* ======================== ICallback Implementation ======================== */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ---------------------------------- dtor ---------------------------------- */

template <class T>
ICallback<T>::~ICallback()
{
    Unsubscribe();
}

/* ---------------------------- member functions ---------------------------- */

template <class T>
void ICallback<T>::Unsubscribe()
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::UNSUBSCRIBE);
    #endif

    if(nullptr != m_dispatcher)
    {
        m_dispatcher->Detach(this);
        m_dispatcher = nullptr;
    }
}

template <class T>
void ICallback<T>::Subscribe(Dispatcher<T>* disp)
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::SUBSCRIBE);
    #endif
    m_dispatcher = disp;
}

template <class T>
void ICallback<T>::NotifyDeath()
{
    #ifndef NDEBUG
        g_logger->Log(DISPATCHER_MSG::DEATH);
    #endif
    
    m_dispatcher = nullptr;
    OnNotifyDeath();
}

template <class T>
void ICallback<T>::OnNotifyDeath()
{
    
}

}
#endif //__ILRD_RD161_DISPATCHER_HPP__


