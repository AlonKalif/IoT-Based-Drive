/*
    Developer: Alon Kalif
    File:      master-minion_messages.hpp
    Reviewer:  
    Date: 	   20.01.2025
    Status:    In progress
    Version:   1.0
*/

#ifndef ILRD_RD161_MASTER_MINION_MESSAGES_HPP
#define ILRD_RD161_MASTER_MINION_MESSAGES_HPP

#include <memory>               // For shared_ptr

#include "atransmittable.hpp"   // To make classes network Transmittable
#include "cpp_uid.hpp"          // For AMsg Data member 

namespace ilrd
{

class AMsg : public ATransmittable
{
public:
    enum MsgType
    {
        READ_CMD,
        WRITE_CMD,
        READ_RESPONSE,
        WRITE_RESPONSE
    };

    AMsg() = default;
   ~AMsg() = 0;
    AMsg(MsgType, const Uid&);

    char*    ToBuffer   (char* buffer) const override;
    char*    FromBuffer (char* buffer)       override;
    uint32_t GetObjSize ()             const override;

    MsgType GetMsgType () const;
    Uid     GetUid     () const;

private:
    int32_t m_msgType;
    Uid     m_uid;
};

/* ---------------------------- Master To Minion ---------------------------- */

class ACmdMsg : public AMsg
{
public:
    ACmdMsg() = default;
   ~ACmdMsg() = 0;
    ACmdMsg(MsgType, const Uid&, uint64_t offset, uint32_t numBytes);

    uint32_t GetObjSize ()             const override;
    char*    ToBuffer   (char* buffer) const override;
    char*    FromBuffer (char* buffer)       override;

    uint64_t GetOffset   () const;
    uint32_t GetNumBytes () const;

private:
    uint64_t m_offset;
    uint32_t m_numBytes;
};

class ReadCmdMsg : public ACmdMsg
{
public:
    ReadCmdMsg() = default;
   ~ReadCmdMsg() = default;
    ReadCmdMsg(const Uid& uid, uint64_t offset, uint32_t numBytesToRead);

    static std::shared_ptr<AMsg> Creator();
};

class WriteCmdMsg : public ACmdMsg
{
public:
    WriteCmdMsg() = default;
   ~WriteCmdMsg() = default;
    WriteCmdMsg(const Uid& uid, uint64_t offset, uint32_t numBytesToWrite, std::shared_ptr<char> dataToWrite);

    char* ToBuffer   (char* buffer) const override;
    char* FromBuffer (char* buffer)       override;

    void SetDataToWrite(std::shared_ptr<char> dataToWrite);
    std::shared_ptr<char> GetDataToWrite() const;

    static std::shared_ptr<AMsg> Creator();

private:
    std::shared_ptr<char> m_dataToWrite;
};

/* ---------------------------- Minion To Master ---------------------------- */

class ResponseMsg : public AMsg
{
public:

    enum ResponseStatus
    {
        SUCCESS,           
        FAILURE
    };

    ResponseMsg() = default;
   ~ResponseMsg() = default;
    ResponseMsg(MsgType, const Uid&, ResponseStatus, uint32_t responseLen, std::shared_ptr<char> responseData);

    char*      ToBuffer   (char* buffer) const override;
    char*      FromBuffer (char* buffer)       override;
    uint32_t   GetObjSize ()             const override;

    ResponseStatus        GetStatus       () const;
    uint32_t              GetResponseLen  () const;
    std::shared_ptr<char> GetResponseData () const;

    static std::shared_ptr<AMsg> Creator();

private:

    int32_t               m_status;
    uint32_t              m_responseLen;
    std::shared_ptr<char> m_responseData;
};

} // namespace ilrd

#endif	// ILRD_RD161_MASTER_MINION_MESSAGES_HPP 
