/*
    Developer: Alon Kalif
    File:      ICommand.hpp
    Reviewer:  
    Date: 	   31.12.2024
    Status:    In progress
    Version:   1.0
*/
#ifndef ILRD_RD161_ICOMMAND_HPP
#define ILRD_RD161_ICOMMAND_HPP

#include <chrono>       // For milliseconds
#include <functional>   // For std::function
#include <memory>       // For shared_ptr

#include "ICommandData.hpp"

namespace ilrd
{

// K specifies the type of the Factory key
template <typename K>
class ICommand
{
public:

    virtual std::pair< std::function<bool()>, std::chrono::milliseconds> Run(std::shared_ptr<ICommandData<K>>) = 0;
    virtual ~ICommand() = default;
};

}
#endif // ILRD_RD161_ICOMMAND_HPP
