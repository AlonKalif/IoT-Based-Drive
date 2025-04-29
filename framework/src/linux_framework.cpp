#define __HANDLETON__

#include "handleton.hpp"
#include "framework.hpp"
#include "thread_pool.hpp"
#include "reactor.hpp"
#include "scheduler.hpp"
#include "logger.hpp"
#include "input_mediator.hpp"
#include "IInputProxy.hpp"
#include "ICommand.hpp"
#include "ICommandData.hpp"

namespace ilrd
{
    template class Framework     <int, int>;
    template class Reactor       <int>;
    template class InputMediator <int, int>;
    template class IInputProxy   <int, int>;
    template class ICommandData  <int>;
    template class ICommand      <int>;

    template class Singleton <ThreadPool>;
    template class Singleton <Scheduler>;
    template class Singleton <Logger>;

    Logger*     g_logger     = Singleton<Logger>     ::GetInstance();
    ThreadPool* g_threadpool = Singleton<ThreadPool> ::GetInstance();
    Scheduler*  g_scheduler  = Singleton<Scheduler>  ::GetInstance();
}