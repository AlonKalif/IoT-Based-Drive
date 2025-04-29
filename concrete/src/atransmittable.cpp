/*
    Developer: Alon Kalif
    File:      atransmittable.hpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    In progress
    Version:   1.0
*/

#include "atransmittable.hpp"

namespace ilrd
{

ATransmittable::ATransmittable() : m_buffSize(sizeof(m_buffSize))
{
    // MIL
}

ATransmittable::~ATransmittable() = default;    

char* ATransmittable::ToBuffer(char* buffer) const
{
    *(uint32_t*)buffer = m_buffSize;
    buffer += sizeof(uint32_t);

    return buffer;
}

char* ATransmittable::FromBuffer(char* buffer)
{
    m_buffSize = *(uint32_t*)buffer;
    buffer += sizeof(uint32_t);

    return buffer;
}

uint32_t ATransmittable::GetBufferSize() const
{
    return m_buffSize;
}

uint32_t ATransmittable::GetObjSize() const
{
    return sizeof(m_buffSize);
}

void ATransmittable::UpdateBufferSize(uint32_t addToBuffSize)
{
    m_buffSize += addToBuffSize;
}


}   // namespace ilrd