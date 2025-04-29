
#define __HANDLETON__
#include "factory.hpp"
#include "master-minion_messages.hpp"   // For AMsg definition

namespace ilrd
{
    template class Singleton<Factory<int, AMsg>>;
    Factory<int, AMsg>* g_amsgFactory = Singleton<Factory<int, AMsg>>::GetInstance();
}