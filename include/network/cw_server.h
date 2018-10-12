#ifndef CW_SERVER_H
#define CW_SERVER_H

#include <QTimer>
#include "include/network/cw_socket.h"
#include "include/network/cw_helpers.h"

class CServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit CServer(QObject *pParent = nullptr);
    virtual ~CServer();

    bool Setup(std::string sAddr = "0.0.0.0", u16 uPort = 30113);

    bool SendHandshake(u32 uClientUID);
    bool VerifyHandshake(SPacket lPacket, CTcpSocket *pClient = nullptr);

    void Broadcast(EPacketType eType = Log_Packet, QByteArray* pPayload = nullptr);
    bool Transmit(u32 uClient, EPacketType eType = Log_Packet, QByteArray* pPayload = nullptr);

    void FlushAll();

protected:
    void incomingConnection(qintptr socketDescriptor);

protected slots:
    void NewClient();
    void HandleInput(CTcpSocket *pClient, QByteArray* pData = nullptr);

signals:
    void SendCommand(SCommand lCmd);
    void NewClientVerified(u32 uClient);

protected:
    QByteArray* EncodeClientUID(u32 uUID, byte bEncodeByte = 'W');

private:
    QTimer *mpPulse; //!< This is the heartbeat timer, or "pulse". It's job is to heartbeat to the clients.
    const int c_miPulseRate = 1000; //!< The rate to send HEARTBEATs at, in milliseconds.

    std::queue<CTcpSocket*> mqNewClients;
    std::map<u32, CTcpSocket*> mmClients;
};

#endif // CW_SERVER_H
