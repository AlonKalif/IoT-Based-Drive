/*
    Developer: Alon Kalif
    File:      select_listener.hpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_SELECT_LISTENER_HPP
#define ILRD_RD161_SELECT_LISTENER_HPP

#include "reactor.hpp"  // To inherit from Ilistener

namespace ilrd
{

class SelectListener : public Reactor<int>::Ilistener
{
public:
    std::vector<Reactor<int>::ListenPair>
	Listen(const std::vector<Reactor<int>::ListenPair>& listenTo) const override;   // may throw runtime_error
private:
	void ExtractFds(    const std::vector<Reactor<int>::ListenPair>& pairsVec,
                        std::pair<fd_set, fd_set>& fdSetPair_out, int* maxFd_out) const;

	void FillReadyVec( const std::pair<fd_set, fd_set>& fdSetPair,
                       std::vector<Reactor<int>::ListenPair>& vec_out,
                       int maxFd, int numReady) const;
};

} // namespace ilrd

#endif	// ILRD_RD161_SELECT_LISTENER_HPP 
