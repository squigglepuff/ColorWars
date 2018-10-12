#ifndef CW_CLIENT_H
#define CW_CLIENT_H

#include "include/network/cw_socket.h"
#include "include/network/cw_helpers.h"

class CClient : public QObject
{
    Q_OBJECT
public:
    explicit CClient(QObject *pParent = nullptr);
    virtual ~CClient();

    bool Connect(std::string sAddr = "0.0.0.0", u16 uPort = 30113);
    bool Disconnect();

    bool ReplyToHandshake(SPacket lPacket);

    bool Transmit(EPacketType eType = Log_Packet, QByteArray* pPayload = nullptr);

    bool IsAlive();

public slots:
    void Heartbeat();
    void HandleInput(CTcpSocket *pClient, QByteArray* pData = nullptr);

signals:
    void UpdateBoard(std::map<u64, ECellColors> mClrMap);

protected:
    QByteArray* EncodeClientUID(u32 uUID, byte bEncodeByte = 'W');

private:
    CTcpSocket *mpSocket; //!< This is the client socket.
};

#endif // CW_CLIENT_H
