#ifndef CW_SOCKET_H
#define CW_SOCKET_H

// Game globals.
#include "include/network/cw_protocol.h"

enum ESocketState
{
    Unverified_State,
    Handshake_State,
    Verified_State,
    Dead_State
};

// Used to perform low-level communications.
class CTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit CTcpSocket(QObject *pParent = nullptr);
    virtual ~CTcpSocket();

    bool Connect(std::string sAddr = "0.0.0.0", u16 uPort = 30113); // For clients.

    // Getters.
    u32 GetUID();

    u16 GetPort();
    std::string GetIP();
    std::string GetHostname();
    ESocketState GetState();

    void SetState(ESocketState eState = Dead_State);
    void SetHostname(std::string sHostname = "");

public slots:
    void ReadData();
    u64 WriteData(QByteArray *pData = nullptr);
    void HandleError(QAbstractSocket::SocketError sockErr);

signals:
    void DataInput(CTcpSocket* pSelf = nullptr, QByteArray *pData = nullptr);

private:
    ESocketState meState;
    std::string msHostname;
};

#endif // CW_SOCKET_H
