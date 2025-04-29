/*
    Developer: Alon Kalif
    File:      input_mediator.hpp
    Reviewer:  
    Date: 	   30.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_INPUT_MEDIATOR_HPP
#define ILRD_RD161_INPUT_MEDIATOR_HPP

#include <memory>           // For shared_ptr
#include <vector>
#include <tuple>

#include "handleton.hpp"    // For threadpool's instance
#include "logger.hpp"
#include "thread_pool.hpp"
#include "reactor.hpp"
#include "IInputProxy.hpp"
#include "CreateAndExecuteCmd.hpp"  // For CreateAndExecuteCmd class & ICommandData class

namespace ilrd
{

extern Logger* g_logger;
extern ThreadPool* g_threadpool;

// K specifies the type of the Factory key
template <typename fdType, typename K>
class InputMediator
{
public:
    using InputProxy = std::shared_ptr< IInputProxy<fdType, K> >;
    using Mode       = Reactor<fdType>::Mode;

    InputMediator(std::shared_ptr< Reactor<fdType> > reactor,
                  const std::vector< std::tuple< fdType, Mode, InputProxy >>& dataVec);

    class ReactorCallback
    {
    public:
        ReactorCallback(InputProxy inputProxy);

        void operator()(fdType, Mode);
    private:
        InputProxy m_inputProxy;
    };
};


template <typename fdType, typename K>
InputMediator<fdType, K>::InputMediator(std::shared_ptr< Reactor<fdType> > reactor,
                                     const std::vector< std::tuple< fdType, Mode, InputProxy >>& dataVec)
{
    for(auto iter : dataVec)
    {    /*                          fd                 mode              Functor(InputProxy)       */
        (*reactor).Register(std::get<0>(iter), std::get<1>(iter), ReactorCallback(std::get<2>(iter)));
    }
}

template <typename fdType, typename K>
InputMediator<fdType, K>::ReactorCallback::ReactorCallback(InputProxy inputProxy) : m_inputProxy(inputProxy)
{
    // MIL
}

template <typename fdType, typename K>
void InputMediator<fdType, K>::ReactorCallback::operator()(fdType fd, Mode mode)
{
    std::shared_ptr<ICommandData<K>> cmdData = m_inputProxy->GetCommandData(fd, mode);

    if(nullptr != cmdData)
    {
        #ifndef NDEBUG
            g_logger->Log(INPUT_MEDIATOR_MSG::THREAD_POOL_ADD);
        #endif
        
        g_threadpool->Add(std::make_shared<CreateAndExecuteCmd<K>>(CreateAndExecuteCmd<K>(cmdData)));
    }
}

} // namespace ilrd

#endif	// ILRD_RD161_INPUT_MEDIATOR_HPP 
