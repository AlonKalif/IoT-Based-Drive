/*
    Developer: Alon Kalif
    File:      select_listener.cpp
    Reviewer:  
    Date: 	   25.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "select_listener.hpp"
#include "logger.hpp"               // For documentation
#include "concrete_messages.hpp"    // Messages for logger 

namespace ilrd
{

extern Logger* g_logger;

std::vector<Reactor<int>::ListenPair> 
SelectListener::Listen				  
(const std::vector<Reactor<int>::ListenPair>& listenTo) const
{	
	int maxFd = -1;
	// 		   read    write
	std::pair<fd_set, fd_set> fdSetPair;
	ExtractFds(listenTo, fdSetPair, &maxFd);

	int numReady = select(maxFd + 1, &fdSetPair.first, &fdSetPair.second, NULL, NULL);
	if(-1 == numReady)
	{
		// how to handle ? ¯\_(ツ)_/¯
		g_logger->Log(LISTENER_MSG::SELECT_ERROR + " errno = " + std::to_string(errno), __FILE__, __LINE__);
		throw std::runtime_error(LISTENER_MSG::SELECT_ERROR);
	}

	std::vector<Reactor<int>::ListenPair> ready;
	FillReadyVec(fdSetPair, ready, maxFd, numReady);

	return ready;
}

void SelectListener::ExtractFds
(const std::vector<Reactor<int>::ListenPair>& pairsVec,
 std::pair<fd_set, fd_set>& fdSetPair_out, int* maxFd_out) const
{
	for(auto fdModePair : pairsVec)
	{
		switch (fdModePair.second)
		{
		case Reactor<int>::READ:

			FD_SET(fdModePair.first, &fdSetPair_out.first);	
			if(fdModePair.first > *maxFd_out)
			{
				*maxFd_out = fdModePair.first;
			}
			break;

		case Reactor<int>::WRITE:
		
			FD_SET(fdModePair.first, &fdSetPair_out.second);
			if(fdModePair.first > *maxFd_out)
			{
				*maxFd_out = fdModePair.first;
			}	
			break;
		
		default:
        	g_logger->Log(LISTENER_MSG::SELECT_SWITCH_ERROR);
			break;
		}
	}
}

void SelectListener::FillReadyVec
(const std::pair<fd_set, fd_set>& fdSetPair,
 std::vector<Reactor<int>::ListenPair>& vec_out, int maxFd, int numReady) const
{
	for(int i = 0; (i <= maxFd) && (0 < numReady); ++i)
	{
		if(FD_ISSET(i, &fdSetPair.first))
		{
			vec_out.emplace_back(std::make_pair(i, Reactor<int>::READ));
			--numReady;
		}
		else if(FD_ISSET(i, &fdSetPair.second))
		{
			vec_out.emplace_back(std::make_pair(i, Reactor<int>::WRITE));
			--numReady;
		}
	}
}

} // namespace ilrd





