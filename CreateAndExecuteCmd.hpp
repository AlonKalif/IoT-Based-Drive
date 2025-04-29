/*
    Developer: Alon Kalif
    File:      CreateAndExecuteCmd.hpp
    Reviewer:  
    Date: 	   31.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_CREATE_AND_EXECUTE_CMD_HPP
#define ILRD_RD161_CREATE_AND_EXECUTE_CMD_HPP

#include <memory>               // For shared_ptr

#include "handleton.hpp"        // For Factory instance
#include "factory.hpp"          // For Factory functions
#include "thread_pool.hpp"      // For ITPTask
#include "async_injection.hpp"  // To handle response
#include "ICommandData.hpp"
#include "ICommand.hpp" 
#include "message_manager.hpp"

namespace ilrd
{

extern Logger* g_logger;

template <class K, class F, class ...Args>
static Factory<K, ICommand<K>>* g_factory = Singleton<Factory<K, ICommand<K>>>::GetInstance();

template<class K>
class CreateAndExecuteCmd : public ThreadPool::ITPTask
{
public:
    CreateAndExecuteCmd(std::shared_ptr<ICommandData<K>> cmdData);
    void operator()() override;

private:
    std::shared_ptr<ICommandData<K>> m_cmdData;
};

template<class K>
CreateAndExecuteCmd<K>::CreateAndExecuteCmd(std::shared_ptr<ICommandData<K>> cmdData) : m_cmdData(cmdData)
{
    // MIL
}

template<class K>
void CreateAndExecuteCmd<K>::operator()()
{
    #ifndef NDEBUG
        g_logger->Log(CREATE_AND_EXE_MSG::EXECUTING);
    #endif

    K factoryKey = m_cmdData->GetKey();
    std::shared_ptr<ICommand<K>> cmd = g_factory<K, ICommand<K>>->Create(factoryKey);

    std::pair< std::function<bool()>, std::chrono::milliseconds> responseCheker = cmd->Run(m_cmdData);

    if(nullptr != responseCheker.first)
    {
        #ifndef NDEBUG
            g_logger->Log(CREATE_AND_EXE_MSG::NEW_AI);
        #endif

        new AsyncInjection(responseCheker.first, responseCheker.second);
    }
}

#endif	// ILRD_RD161_CREATE_AND_EXECUTE_CMD_HPP 

} // namespace ilrd