/*
    Developer: Alon Kalif
    File:      cpp_uid.hpp
    Reviewer:  
    Date: 	   24.01.2025
    Status:    Tested
    Version:   1.1 
*/

#ifndef ILRD_RD161_CPP_UID_HPP
#define ILRD_RD161_CPP_UID_HPP

#include "atransmittable.hpp"   // To make Uid network Transmittable
#include "uid.h"                // Reuse C uid implementation

namespace ilrd
{

class Uid : public ATransmittable
{
public:
    Uid();              // may throw runtime_error
   ~Uid() = default;

    struct HashUid      // To make Uid hashable
    {
        size_t operator()(const Uid&) const;
    };

    char*    ToBuffer      (char* buffer) const override;
    char*    FromBuffer    (char* buffer)       override;
    uint32_t GetObjSize    ()             const override;
    
private:
    ilrd_uid_t m_uid;

    friend bool operator==(const Uid&, const Uid&); // To make Uid hashable
    friend bool operator!=(const Uid&, const Uid&); // To make Uid hashable
    friend bool operator< (const Uid&, const Uid&); // To make Uid hashable
};

} // namespace ilrd

#endif	// ILRD_RD161_CPP_UID_HPP 
