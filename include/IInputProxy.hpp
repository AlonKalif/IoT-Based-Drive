/*
    Developer: Alon Kalif
    File:      IInputProxy.hpp
    Reviewer:  
    Date: 	   00.00.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_IINPUTPROXY_HPP
#define ILRD_RD161_IINPUTPROXY_HPP

#include <memory>           // For shared_ptr

#include "reactor.hpp"      // For Reactor::Mode
#include "ICommandData.hpp"

namespace ilrd
{

template <class fdType, typename K>
class IInputProxy
{
public:
    virtual std::shared_ptr<ICommandData<K>> GetCommandData(fdType fd, Reactor<fdType>::Mode mode) = 0;
    virtual ~IInputProxy() = default;
};

} // namespace ilrd

#endif	// ILRD_RD161_IINPUTPROXY_HPP 
