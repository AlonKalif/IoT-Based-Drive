/*
    Developer: Alon Kalif
    File:      atransmittable.hpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    In progress
    Version:   1.0
*/
#ifndef __ILRD_RD161_ATRANSMITTABLE_HPP__
#define __ILRD_RD161_ATRANSMITTABLE_HPP__

#include <stdint.h>     // For uint32_t

namespace ilrd
{

class ATransmittable
{
public:
    ATransmittable();
    virtual ~ATransmittable() = 0;

    virtual char* ToBuffer   (char* buffer) const;
    virtual char* FromBuffer (char* buffer);

    /* 
        GetObjSize() -  Implementation in a derived class should return the 
                        combined size of its data members (in bytes). 
    */
    virtual uint32_t GetObjSize() const;

    /* 
        UpdateBufferSize() -    Every derived class should call this function in
                                its constructor and pass the result of GetObjSize()
                                as an argument.
    */
    void     UpdateBufferSize (uint32_t addToBuffSize);

    uint32_t GetBufferSize() const;
    
private:
    uint32_t m_buffSize;
};

} // namespace ilrd

#endif // __ILRD_RD161_ATRANSMITTABLE_HPP__
