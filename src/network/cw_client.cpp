#include "include/network/cw_client.h"

CClient::CClient(QObject *pParent) : QObject{pParent}, mpSocket{nullptr}
{
    // Intentionally left blank.
}

CClient::~CClient()
{
    Disconnect();
}

bool CClient::Connect(std::string sAddr, u16 uPort)
{
    bool bSuccess = false;

    if (nullptr == mpSocket)
    {
        mpSocket = new CTcpSocket(this);
        mpSocket->connectToHost(QString::fromStdString(sAddr), uPort, QAbstractSocket::ReadWrite, QAbstractSocket::IPv4Protocol);

        u32 uWaitTime = 30000;
        u32 uTimePassed = 0;
        const int c_iInterval = 10;
        do
        {
            if (mpSocket->state() == QAbstractSocket::ConnectedState)
            {
                break;
            }
            else
            {
                bSuccess = mpSocket->waitForConnected(c_iInterval);
                uTimePassed += c_iInterval;
            }
        } while(uTimePassed < uWaitTime && !bSuccess);

        bSuccess = (mpSocket->state() == QAbstractSocket::ConnectedState);

        if (bSuccess)
        {
            connect(mpSocket, &CTcpSocket::readyRead, mpSocket, &CTcpSocket::ReadData);
            connect(mpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), mpSocket, &CTcpSocket::HandleError);
            connect(mpSocket, &CTcpSocket::DataInput, this, &CClient::HandleInput);

            mpSocket->SetState(Unverified_State);
        }
        else
        {
            qCritical("Timed out connecting to %s:%u! Maybe the address/port is wrong?", sAddr.c_str(), uPort);
        }
    }
    else
    {
        qCritical("Already connected! Close the current connection before opening a new one!");
    }

    return bSuccess;
}

bool CClient::Disconnect()
{
    if (nullptr != mpSocket)
    {
        if (mpSocket->isOpen()) { mpSocket->close(); }
        delete mpSocket;
        mpSocket = nullptr;
    }

    return (nullptr == mpSocket);
}

bool CClient::ReplyToHandshake(SPacket lPacket)
{
    bool bSuccess = false;

    if (IsAlive())
    {
        QByteArray *pNeeded = EncodeClientUID(CW_NET_SERVERID);

        qDebug("Challenge: %s", pNeeded->toHex().toStdString().c_str());
        qDebug("Response: %s", lPacket.mpPayload->toHex().toStdString().c_str());

        if (nullptr != lPacket.mpPayload && lPacket.mpPayload->toHex() == pNeeded->toHex())
        {
            bSuccess = Transmit(Handshake_Packet, EncodeClientUID(mpSocket->GetUID()));

            if (bSuccess) { mpSocket->SetState(Handshake_State); }
        }

        if (nullptr != pNeeded) { delete pNeeded; }
    }

    return bSuccess;
}

bool CClient::Transmit(EPacketType eType, QByteArray* pPayload)
{
    bool bSuccess = false;
    if (IsAlive())
    {
        SPacket lPacket;
        lPacket.msID = "CwNp";
        lPacket.muClientUID = mpSocket->GetUID();
        lPacket.meType = eType;
        lPacket.mpPayload = pPayload;

        QByteArray *pPacket = CProtocol::CreatePacket(lPacket);
        if (nullptr != pPacket)
        {
            bSuccess = (pPacket->length() <= mpSocket->WriteData(pPacket));
            delete pPacket;
        }
    }
    else
    {
        qCritical("Unable to transmit on a null connection! Please set one up first!");
    }

    return bSuccess;
}

bool CClient::IsAlive()
{
    return (nullptr != mpSocket && mpSocket->isOpen());
}

void CClient::FlushAll()
{
    if (nullptr != mpSocket)
    {
        mpSocket->flush();
    }
}

void CClient::Heartbeat()
{
    Transmit(Heartbeat_Packet, new QByteArray("~$$HEARTBEAT"));
}

void CClient::HandleInput(CTcpSocket* pClient, QByteArray* pData)
{
    if (nullptr != pData && CProtocol::IsValid(pData) && nullptr != pClient)
    {
        // Parse the input packet.
        SPacket lPacket = CProtocol::ParsePacket(pData);

        switch (lPacket.meType)
        {
            case Heartbeat_Packet:
            {
                if (mpSocket->GetState() == Handshake_State) { mpSocket->SetState(Verified_State); }
                if (mpSocket->GetState() == Verified_State)
                {
                    qDebug("Heartbeat: server --> client [GOOD]");
                    Transmit(Heartbeat_Packet, new QByteArray("~$$HEARTBEAT"));
                }
                break;
            }
            case Command_Packet:
            {
                if (mpSocket->GetState() == Verified_State) { qWarning("We...don't take commands, we give 'em >:|"); }
                break;
            }
            case Update_Packet:
            {
                if (mpSocket->GetState() == Verified_State)
                {
                    emit UpdateBoard(UnpackBoardMap(lPacket.mpPayload));
                }
                break;
            }
            case Log_Packet:
            {
                if (mpSocket->GetState() == Verified_State)
                {
                    HandleNetLogging(lPacket.mpPayload->toStdString());
                }
                break;
            }
            case Handshake_Packet:
            {
                if (mpSocket->GetState() == Unverified_State || mpSocket->GetState() == Handshake_State) { ReplyToHandshake(lPacket); }
                break;
            }
            case Server_HostQuery:
            {
                if (mpSocket->GetState() == Verified_State || mpSocket->GetState() == Handshake_State)
                {
                    qInfo("Server wants our hostname.");
                    if (!Transmit(Client_HostResponse, new QByteArray(mpSocket->GetHostname().c_str())))
                    {
                        qWarning("Was unable to respond to server's query about our hostname!");
                    }
                    else
                    {
                        qInfo("Sent server our hostname.");
                    }
                }
                break;
            }
            default: /* case Unknown_Packet: */
            {
                qCritical("Received unknown or invalid packet from %s [%u]! Ignoring...", pClient->GetIP().c_str(), pClient->GetUID());
                break;
            }
        }
    }
    else
    {
        qCritical("Received invalid packet data! Discarding...");
    }
}

QByteArray* CClient::EncodeClientUID(u32 uUID, byte bEncodeByte)
{
    QByteArray *pData = new QByteArray();
    pData->append(static_cast<char>( (uUID & 0xff000000) >> 24) );
    pData->append(static_cast<char>( (uUID & 0x00ff0000) >> 16) );
    pData->append(static_cast<char>( (uUID & 0x0000ff00) >> 8) );
    pData->append(static_cast<char>( (uUID & 0x000000ff)) );

    for (int iIdx = 0; iIdx < pData->size(); ++iIdx)
    {
        pData->replace(pData->at(iIdx), static_cast<char>(pData->at(iIdx) ^ bEncodeByte));
    }

    return pData;
}
