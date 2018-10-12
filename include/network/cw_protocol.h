#ifndef CW_PROTOCOL_H
#define CW_PROTOCOL_H

#include "include/globals.h"

#define CW_NET_SERVERID (0xffaaeedd)

enum EPacketType
{
    Heartbeat_Packet = 0xfac0,
    Command_Packet = 0xfac1,
    Update_Packet = 0xfac2,
    Log_Packet = 0xfac3,
    Handshake_Packet = 0xface,

    // Server specific packets.
    Server_HostQuery = 0xffc0,

    // Client specific packets.
    Client_HostResponse = 0xaac0,

    Unknown_Packet = 0xfacf
};

struct SPacket
{
    std::string msID;
    u32 muClientUID;
    EPacketType meType;
    QByteArray* mpPayload;

    SPacket(std::string sID = "", u32 uUID = 0, EPacketType eType = Unknown_Packet, QByteArray* pPayload = nullptr) : msID{sID}, muClientUID{uUID}, meType{eType}, mpPayload{pPayload} {}
};

/*!
 * \brief The CProtocol class
 *
 * This class never needs to be instantiated, it's used as a container for like-functions. THese functions are simply here to provide an easy-to-use interface for protocol (un)packing.
 */
class CProtocol
{
    CProtocol();
    ~CProtocol();
public:
    static QByteArray* CreatePacket(SPacket lPacket);
    static SPacket ParsePacket(QByteArray* pData = nullptr);

    static bool IsValid(QByteArray* pPacket = nullptr);
    static bool IsValid(SPacket lPacket);

    static QByteArray* XorData(QByteArray* pData = nullptr);

    static const char* ProtocolEOF();
};

#endif // CW_PROTOCOL_H
