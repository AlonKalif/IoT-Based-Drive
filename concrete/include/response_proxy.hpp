/*
    Developer: Alon Kalif
    File:      response_proxy.hpp
    Reviewer:  
    Date: 	   30.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_RESPONSE_PROXY_HPP
#define ILRD_RD161_RESPONSE_PROXY_HPP

#include <unordered_map>

#include "IInputProxy.hpp"  // To inherit from IInputProxy

namespace ilrd
{

class MinionProxy;

class ResponseProxy : public IInputProxy<int, int>
{
public:
    ResponseProxy(MinionProxy*);
    std::shared_ptr<ICommandData<int>> GetCommandData(int fd, Reactor<int>::Mode mode) override;

private:
    MinionProxy* m_minionProxy;

    void BadStatusHandler();
};

} // namespace ilrd

#endif	// ILRD_RD161_RESPONSE_PROXY_HPP 
