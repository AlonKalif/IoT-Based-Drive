#ifndef ILRD_RD161_MSG_MANNGER_HPP
#define ILRD_RD161_MSG_MANNGER_HPP

#include <string>

namespace DISPATCHER_MSG
{
    const std::string NOTIFY = "Dispatcher::Notify";
    const std::string ATTACH = "Dispatcher::Attach";
    const std::string DETACH = "Dispatcher::Detach";
    const std::string SUBSCRIBE = "Dispatcher - callback subscribed";
    const std::string UNSUBSCRIBE = "Dispatcher - callback Unsubscribed";
    const std::string DEATH = "Dispatcher - NotifyDeath";
}

namespace CREATE_AND_EXE_MSG
{
    const std::string EXECUTING = "Executing ITPTask in thread pool";
    const std::string NEW_AI = "creating new Async Injection task";
}

namespace FACTORY_MSG
{
    const std::string NEW_PRODUCT = "New object made by factory";
    const std::string NEW_PRODUCT_ERROR = "ERROR - Factory failed to create object\nout of range exception";
    const std::string NEW_CREATOR = "New creator added to factory";
}

namespace FRAMEWORK_MSG
{
    const std::string RUN = "Framework running";
    const std::string STOP = "Framework stopped";

}

namespace INPUT_MEDIATOR_MSG
{
    const std::string THREAD_POOL_ADD = "Input mediator - adding new thread pool task";
}

namespace REACTOR_MSG
{
    const std::string RUN = "Reactor running";
    const std::string STOP = "Reactor stopped";
    const std::string EVENT = "Reactor - event occured";
    const std::string REGISTER = "Reactor - new handler registered";
    const std::string UNREGISTER = "Reactor - handler unregistered";
}

namespace TP_MSG
{
    const std::string FUTURE_OP = "threadpool - executing future task";
    const std::string FUTURE_DONE = "threadpool - future task done";
    const std::string THREAD_RUN = "threadpool - new thread running";
    const std::string THREAD_STOP = "threadpool - thread stopped";

    const std::string NEW_TASK = "threadpool - new task added";
    const std::string TP_INIT = "threadpool - initializing, num threads: ";
    const std::string TP_DESTROYED = "threadpool - destroyed";
    const std::string SET_NUM_THREADS = "threadpool - num threads changed to: ";    

    const std::string ADMIN_STOP = "threadpool - executing admin stop task";
    const std::string ADMIN_BLANK = "threadpool - executing admin blank task";
}

namespace SCHEDULER_MSG
{
    const std::string ADD_ERROR = "ERROR - Scheduler - invalid argument";
}

namespace DIR_MONITOR_MSG
{
    const std::string FOLDER = "Dir monitor - Monitoring folder: ";
    const std::string FOLDER_DONE = "Dir monitor - Done monitoring folder: ";
    const std::string REGISTER = "Dir monitor - Callback registered";
    const std::string REGISTER_ERROR = "ERROR - Dir monitor - Invalid argument passed to Register";
    const std::string UNREGISTER = "Dir monitor - Callback unregistered";
    const std::string UNREGISTER_ERROR = "ERROR - Dir monitor - Invalid argument passed to UnRegister";
    const std::string EVENT_LOOP_ERROR = "ERROR - Dir monitor - Inotify failed to read event";
    const std::string DIR_MONITOR_DESTRUCTION_ERROR = "ERROR - Dir monitor - Inotify didn't close properly";
}

namespace DLL_LOADER_MSG
{
    const std::string LOADER_INIT_ERROR = "ERROR - Dll loader - Invalid argument passed to DllLoader";
    const std::string DLOPEN_ERROR = "ERROR - Dll loader - dlopen failed to load: ";
    const std::string LOADED = "Dll loader - Successfully loaded: ";
    const std::string FAILED_TO_VERIFY_ERROR = "ERROR - Dll loader- failed to verify file type";
}

namespace AI_MSG
{
    const std::string AI_INIT = "New async injection creation";
}

#endif // ILRD_RD161_MSG_MANNGER_HPP