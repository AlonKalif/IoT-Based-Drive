
#define __HANDLETON__
#include "factory.hpp"
#include "ICommand.hpp"

namespace ilrd
{
    template class Singleton<Factory<int, ICommand<int>>>;
    Factory<int, ICommand<int>>* g_factory = Singleton<Factory<int, ICommand<int>>>::GetInstance();
}