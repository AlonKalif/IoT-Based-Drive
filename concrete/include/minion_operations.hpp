/*
    Developer: Alon Kalif
    File:      minion_operations.hpp
    Reviewer:  
    Date: 	   27.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_MINION_OPERATIONS_HPP
#define ILRD_RD161_MINION_OPERATIONS_HPP

#include "ICommand.hpp"         // To inherit from ICommand
#include "minion_cmd_data.hpp"  // Data containers

namespace ilrd
{

class ReadOperation : public ICommand<int>
{
public:
	std::pair< std::function<bool()>, std::chrono::milliseconds>
	    Run(std::shared_ptr<ICommandData<int>> cmdData) override; // may throw invalid_argument

    static std::shared_ptr<ICommand<int>> Creator();
    static void Config(size_t memSize);

private:
    static size_t s_memSize;

    std::shared_ptr<char> PerformRead(ReadOperationData* data);
};

class WriteOperation : public ICommand<int>
{
public:
	std::pair< std::function<bool()>, std::chrono::milliseconds>
	    Run(std::shared_ptr<ICommandData<int>> operationData) override; // may throw invalid_argument

    static std::shared_ptr<ICommand<int>> Creator();
    
    static void Config(size_t memSize);

private:
    static size_t s_memSize;

    ResponseMsg::ResponseStatus PerformWrite(WriteOperationData* data);
};

} // namespace ilrd

#endif	// ILRD_RD161_MINION_OPERATIONS_HPP 
