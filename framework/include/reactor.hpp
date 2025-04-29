/*
    Developer: Alon Kalif
    File:      reactor.hpp
    Reviewer:  Shimon
    Date: 	   24.12.2024
    Status:    In progress
    Version:   1.1 new hash function
*/
#ifndef __ILRD_REACTOR_HPP__
#define __ILRD_REACTOR_HPP__

#include <functional>       // For std::function
#include <memory>           // For std::shared_ptr
#include <atomic>           // To allow Stop() from another thread
#include <vector>
#include <unordered_map>

#include "logger.hpp"
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

template<typename fdType>
class Reactor
{
public:
    enum Mode
    {
        READ, 
        WRITE
    };

    using ListenPair = std::pair<fdType, Reactor::Mode>;
    
    class Ilistener
    {
    public:
        virtual ~Ilistener() = default;
        virtual std::vector<ListenPair> Listen(const std::vector<ListenPair>& listenTo) const = 0;
    };
    
    explicit Reactor(std::shared_ptr<Ilistener> listen);
            ~Reactor() = default;

             Reactor  (const Reactor&) = delete;
    Reactor& operator=(const Reactor&) = delete;

    void Register  (fdType fd, Mode mode, std::function<void(fdType, Mode)> handler); // may throw bad_alloc
    void UnRegister(fdType fd, Mode mode);
    
    void Run(); // blocking    
    void Stop();
    
private:
    struct HashPair
    {
        size_t operator()(const ListenPair& p) const
        {
            std::hash<int> intHash;
            size_t hash1 = intHash(p.first);
            size_t hash2 = intHash(static_cast<int>(p.second));

            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };

    std::unordered_map
                    <
                    ListenPair,
                    std::function<void(fdType, Mode)>,
                    HashPair
                    >
                    m_umap;

    std::shared_ptr<Ilistener> m_listener;
    std::atomic<bool> m_isRunning;
};// class Reactor

/* ============================= Public Methods ============================= */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ------------------------------ ctors & dtor ------------------------------ */

template<typename fdType>
Reactor<fdType>::Reactor(std::shared_ptr<Ilistener> listen) :
                                            m_listener(listen), m_isRunning(true)
{

}

/* ---------------------------- member functions ---------------------------- */

template<typename fdType>
void Reactor<fdType>::Run()
{
    #ifndef NDEBUG
        g_logger->Log(REACTOR_MSG::RUN);
    #endif

    while(m_isRunning)
    {
        std::vector<ListenPair> listeners;

        for(auto iter : m_umap)
        {
            listeners.emplace_back(iter.first);
        }

        std::vector<ListenPair> ready = (*m_listener).Listen(listeners); // Blocking

        #ifndef NDEBUG
            g_logger->Log(REACTOR_MSG::EVENT);
        #endif

        for(auto iter : ready)
        {
            if(m_umap.contains(iter))
            {
                m_umap[iter](iter.first, iter.second); // invoke callback
            }
        }
    }
}

template<typename fdType>
void Reactor<fdType>::Stop()
{
    #ifndef NDEBUG
        g_logger->Log(REACTOR_MSG::STOP);
    #endif
        
    m_isRunning = false;
}

template<typename fdType>
void Reactor<fdType>::Register(fdType fd, Mode mode, std::function<void(fdType, Mode)> handler)
{
    m_umap.emplace(std::make_pair(fd, mode), handler);

    #ifndef NDEBUG
        g_logger->Log(REACTOR_MSG::REGISTER);
    #endif
}

template<typename fdType>
void Reactor<fdType>::UnRegister(fdType fd, Mode mode)
{
    m_umap.erase(std::make_pair(fd, mode));

    #ifndef NDEBUG
        g_logger->Log(REACTOR_MSG::UNREGISTER);
    #endif
}


} // namespace ilrd

#endif //__ILRD_REACTOR_HPP__