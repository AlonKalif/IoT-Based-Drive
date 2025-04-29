#ifndef ILRD_RD161_CONCRETE_MESSAGES_HPP
#define ILRD_RD161_CONCRETE_MESSAGES_HPP

#include <string>

namespace CONFIG_MSG
{
}

namespace NBD_PROXY_MSG
{
    const std::string RECEIVED_READ_REQUEST = "NBDProxy received new read request";
    const std::string RECEIVED_WRITE_REQUEST = "NBDProxy received new write request";
    const std::string MSG_TYPE_ERROR = "ERROR - NBDProxy::CreateCommandData - received unfamiliar request type";
}

namespace TRANSMITTABLE_MSG
{
    const std::string NEW_READ_CMD = "new ReadCmdMsg created";
    const std::string NEW_WRITE_CMD = "new WriteCmdMsg created";
    const std::string NEW_RESPONSE = "new ResponseMsg created";
}

namespace RESPONSE_MANAGER_MSG
{
    const std::string CMD_REGISTERED = "new command registered to ResponseManager";
    const std::string RESPONSE_REGISTERED = "new response registered to ResponseManager";
    const std::string REGISTER_RESPONSE_FAIL = "unregistered uid passed to ResponseManager::RegisterResponse";
}

namespace UID_MSG
{
    const std::string UID_CREATE_ERROR = "ERROR - Uid ctor - failed to create new uid";
}

namespace RAID_MSG
{
    const std::string OFFSET_ERROR = "ERROR - RaidManager::OffsetToProxy - offset out of bounds";
    const std::string PROXY_INIT_ERROR = "ERROR - RaidManager::Ctor - minion proxy failed to init. ";
}

namespace COMMAND_MSG
{
    const std::string OFFSET_ERROR = "ERROR - ICommand::Run - RaidManager didn't accept offset";
    const std::string WRITE_CMD_ARG_ERROR = "ERROR - WriteCommand::Run - got pointer to data of incorrect type";
    const std::string READ_CMD_ARG_ERROR = "ERROR - ReadCommand::Run - got pointer to data of incorrect type";
}

namespace LISTENER_MSG
{
    const std::string SELECT_SWITCH_ERROR = "ERROR - SelectListener::ExtractFds - unexpected Reactor mode";
    const std::string SELECT_ERROR = "ERROR - SelectListener::Listen - select returned -1";
}

namespace TCP_MANAGER_MSG
{
    const std::string SOCKET_INIT_ERROR = "ERROR - TcpManager Ctor - socket() returned -1";
    const std::string SOCKET_BIND_ERROR = "ERROR - TcpManager Ctor - bind() returned -1";
    const std::string SOCKET_LISTEN_ERROR = "ERROR - TcpManager Ctor - listen() returned -1";
    const std::string SOCKET_ACCEPT_ERROR = "ERROR - TcpManager AcceptTcpClient - accept() returned -1";
    const std::string SOCKET_SEND_ERROR = "ERROR - TcpManager SendMsg - send () returned -1";
    const std::string SOCKET_RECV_ERROR = "ERROR - TcpManager RecvMsg - recv() returned -1";

    const std::string CLIENT_DISCONNECTED = "TcpManager RecvMsg - client disconnected";
    const std::string CLIENT_ACCEPTED = "TcpManager AcceptTcpClient - new client accepted";
}

namespace UDP_SOCKET_MSG
{
    const std::string SOCKET_INIT_ERROR = "ERROR - UdpSocket Ctor - socket() returned -1";
    const std::string SOCKET_BIND_ERROR = "ERROR - UdpSocket Ctor - bind() returned -1";
    const std::string SOCKET_SEND_ERROR = "ERROR - UdpSocket SendMsg - sendto() returned -1";
    const std::string SOCKET_RECV_ERROR = "ERROR - UdpSocket RecvMsg - recvfrom() returned -1";
}

namespace SOCKET_ADDR_MSG
{
    const std::string SOCKET_ADDR_INIT_ERROR = "ERROR - SocketAddr Ctor - Invalid IP string passed. inet_pton failed due to invalid IP string";
}

#endif // ILRD_RD161_CONCRETE_MESSAGES_HPP