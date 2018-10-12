#ifndef NETWORK_H
#define NETWORK_H
#if 0
class CNetwork : public QObject
{
    Q_OBJECT
public:
    explicit CNetwork(QObject* pParent = nullptr);
    virtual ~CNetwork();

    bool ConnectToServer(std::string sAddr = "127.0.0.1", u16 uPort = 30113);
    bool SetupServer(std::string sAddr = "127.0.0.1", u16 uPort = 30113);

    void Heartbeat();

    QByteArray* PackBoardMap(std::map<u64, ECellColors> mData);
    std::map<u64, ECellColors> UnpackBoardMap(QByteArray* pData = nullptr);

public slots:
    void NewData(QByteArray *pData = nullptr);
    void SendData(QByteArray *pData = nullptr);

signals:
    void SendInternalCommand(SCommand lCmd);
    void UpdateBoardData(std::map<u64, ECellColors> lClrMap);
    void UpdateLog(std::string lMsg);

private:
    QByteArray* XorData(QByteArray* pData = nullptr);
    QByteArray* BuildPacketHeader(u32 uUID, EPacketType eType);

    CUdpSocket *mpSocket;
    std::vector<u32> mvClients;
};
#endif

#include "include/network/cw_socket.h"
#include "include/network/cw_protocol.h"
#include "include/network/cw_server.h"
#include "include/network/cw_client.h"

#endif // NETWORK_H
