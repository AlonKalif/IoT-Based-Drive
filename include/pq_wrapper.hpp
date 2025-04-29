/*
    Developer: Alon Kalif
    File:      pq.hpp
    Reviewer:  
    Date: 	   00.00.2024
    Status:    In progress
    Version:   1.0
*/
#ifndef ILRD_RD161_PQ_HPP
#define ILRD_RD161_PQ_HPP

#include <queue>

#include "logger.hpp"

namespace ilrd
{

extern Logger* g_logger;

template<typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type>>
class PQueue
{
public:
	void pop_front();
    void push_back(const T& val);
    bool empty() const;	
    const T& front();
private:
	std::priority_queue<T, Container, Compare> m_pq; 
};

template<typename T, typename Container, typename Compare>
void PQueue<T, Container, Compare>::pop_front()
{
    m_pq.pop();
}

template<typename T, typename Container, typename Compare>
void PQueue<T, Container, Compare>::push_back(const T& val)
{
    m_pq.push(val);
}

template<typename T, typename Container, typename Compare>
bool PQueue<T, Container, Compare>::empty() const
{
    return m_pq.empty();
}

template<typename T, typename Container, typename Compare>
const T& PQueue<T, Container, Compare>::front() 
{
    return m_pq.top();
}

} // namespace ilrd


#endif	// ILRD_RD161_PQ_HPP 
