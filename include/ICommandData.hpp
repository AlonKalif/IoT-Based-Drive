/*
    Developer: Alon Kalif
    File:      IKeyArgs.hpp
    Reviewer:  
    Date: 	   30.12.2024
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_ICOMMAND_DATA_HPP
#define ILRD_RD161_ICOMMAND_DATA_HPP

namespace ilrd
{

template <typename K>
class ICommandData
{
public:
    virtual K GetKey() = 0;
    virtual ~ICommandData() = default;
};

} // namespace ilrd

#endif	// ILRD_RD161_ICOMMAND_DATA_HPP 
