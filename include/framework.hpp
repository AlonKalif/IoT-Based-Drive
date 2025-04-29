/*
    Developer: Alon Kalif
    File:      framework.hpp
    Reviewer:  
    Date: 	   30.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_FRAMEWORK_HPP
#define ILRD_RD161_FRAMEWORK_HPP

#include <vector>
#include <memory>       // For shared_ptr
#include <functional>   // For std::function
#include <tuple>

#include "dir_monitor.hpp"
#include "dll_loader.hpp"
#include "reactor.hpp"
#include "ICommand.hpp"
#include "IInputProxy.hpp"
#include "input_mediator.hpp"
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

// maybe make framework singleton, 
// K specifies the type of the Factory key
template <typename fdType, typename K>
class Framework
{
public:
    Framework(const std::vector<std::tuple< fdType, typename Reactor<fdType>::Mode, std::shared_ptr<IInputProxy<fdType, K>> >>& fdCallbacks,
              const std::vector<std::pair< K, std::function<std::shared_ptr<ICommand<K>>()> >>& creators,
              const std::string& pluginDirPath,
              std::shared_ptr<typename Reactor<fdType>::Ilistener> fdListener,
              std::function<void(void)> onDllLoadFailure);

    void Run();     // Blocking
    void Stop();
    
private:
    std::shared_ptr<Reactor<fdType>> m_reactor;
    std::shared_ptr<DirMonitor>      m_dirMonitor;
    std::shared_ptr<DllLoader>       m_dllLoader;
};

template <typename fdType, typename K>
Framework<fdType, K>::Framework(
    // args:
    const std::vector<std::tuple< fdType, typename Reactor<fdType>::Mode, std::shared_ptr<IInputProxy<fdType, K>> >>& fdCallbacks,
    const std::vector<std::pair< K, std::function<std::shared_ptr<ICommand<K>>()> >>& creators,
    const std::string& pluginDirPath,
    std::shared_ptr<typename Reactor<fdType>::Ilistener> fdListener,
    std::function<void(void)> onDllLoadFailure):
    // MIL:
    m_reactor   (std::make_shared<Reactor<fdType>>(fdListener)),
    m_dirMonitor(std::make_shared<DirMonitor>(pluginDirPath)),
    m_dllLoader (std::make_shared<DllLoader>(m_dirMonitor.get(), onDllLoadFailure))
{
    (void)pluginDirPath;

    for(auto iter : creators)
    {
        g_factory<K, ICommand<K>>->Add(iter.first, iter.second); 
    }

    InputMediator(m_reactor, fdCallbacks);
}

template <typename fdType, typename K>
void Framework<fdType, K>::Run()
{
    #ifndef NDEBUG
        g_logger->Log(FRAMEWORK_MSG::RUN);
    #endif

    m_reactor->Run();
}

template <typename fdType, typename K>
void Framework<fdType, K>::Stop()
{
    #ifndef NDEBUG
        g_logger->Log(FRAMEWORK_MSG::STOP);
    #endif

    m_reactor->Stop();
}

} // namespace ilrd

#endif // ILRD_RD161_FRAMEWORK_HPP
