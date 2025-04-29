/*
    Developer: Alon Kalif
    File:      factory.hpp
    Reviewer:  
    Date: 	   13.12.2024
    Status:    In progress
    Version:   1.1      // added logging and singleton
*/
#ifndef __ILRD_RD161_FACTORY_HPP__
#define __ILRD_RD161_FACTORY_HPP__ 

#include <memory>           // std::shared_ptr
#include <functional>       // for std::function
#include <string>           // For std::to_string
#include <unordered_map>

#include "handleton.hpp"        // To make Factory singleton
#include "logger.hpp"           // For documentation
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

template <class K, class F, class ...Args>
class Factory
{
public:
    Factory(const Factory& other) = delete;
    Factory(Factory&& other) = delete;
    Factory& operator=(const Factory& other) = delete;
    ~Factory() = default;

    using FPtr = std::shared_ptr<F>;
    void Add(K key, std::function<FPtr(Args...)> creator); // throws bad_alloc. if key already exists in factory, it will be overwritten.
    FPtr Create(const K& key, Args... args); // throws bad_alloc

private:
    friend class Singleton<Factory>;

    explicit Factory() = default; // throws bad_alloc
    std::unordered_map<K, std::function<FPtr(Args...)>> m_factories;
};

template <class K, class F, class ...Args>
void Factory<K, F, Args...>::Add(K key, std::function<FPtr(Args...)> creator)
{
    m_factories[key] = creator;

    #ifndef NDEBUG
        g_logger->Log(FACTORY_MSG::NEW_CREATOR);
    #endif
}

template <class K, class F, class ...Args>
typename Factory<K, F, Args...>::FPtr Factory<K, F, Args...>::Create(const K& key, Args... args)
{
    FPtr product;

    try
    {
        product = m_factories.at(key)(std::forward<Args>(args)...);
    }
    catch(const std::out_of_range& e)
    {
        g_logger->Log(FACTORY_MSG::NEW_PRODUCT_ERROR, __FILE__, __LINE__);
    }

    #ifndef NDEBUG
        g_logger->Log(FACTORY_MSG::NEW_PRODUCT);
    #endif    

    return product;
}

} // namespace ilrd
#endif //__ILRD_RD161_FACTORY_HPP__
