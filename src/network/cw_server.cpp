#include "include/network/cw_server.h"

CServer::CServer(QObject *pParent) : QTcpServer{pParent}, mpPulse{nullptr}
{
    // Intentionally left blank.
}

CServer::~CServer()
{
    qInfo("Destructing server...");
    if (nullptr != mpPulse)
    {
        if (mpPulse->isActive()) { mpPulse->stop(); }
        delete mpPulse;
        mpPulse = nullptr;
    }

    while (!mmClients.empty())
    {
        std::pair<u32, CTcpSocket*> pTmp = *(mmClients.rbegin());
        if (nullptr != pTmp.second) { delete pTmp.second; }
        mmClients.erase(pTmp.first);
    }
    qInfo("All done, Good-bye!");
}

bool CServer::Setup(std::string sAddr, u16 uPort)
{
    bool bSuccess = false;

    if (0 == sAddr.compare("*"))
    {
        sAddr = "0.0.0.0";
    }

    if (1024 >= uPort)
    {
        qWarning("You've picked a port below the system reserve! (Picked: %u)", uPort);
    }

    qInfo("Attempting to setup TCP server...");
    if (!isListening())
    {
        bSuccess = listen(QHostAddress(QString::fromStdString(sAddr)), uPort);
        if (bSuccess)
        {
            connect(this, &QTcpServer::newConnection, this, &CServer::NewClient);

            // Purely for information, get our local IP.
            std::string sLocalIP = "127.0.0.1";
            if (0 == sAddr.compare("127.0.0.1") || 0 == sAddr.compare("0.0.0.0"))
            {
                foreach (const QHostAddress &cAddr, QNetworkInterface::allAddresses())
                {
                    if (cAddr.protocol() == QAbstractSocket::IPv4Protocol && cAddr != QHostAddress(QHostAddress::LocalHost) && !cAddr.toString().endsWith(".1"))
                    {
                        sLocalIP  = cAddr.toString().toStdString();
                        break;
                    }
                }
            }

            qInfo("Server connected to %s:%u.", sAddr.c_str(), uPort);
            qInfo("Have clients connect to: %s:%u", sLocalIP.c_str(), uPort);
            qInfo("Currently listening connections (%d)...", maxPendingConnections());

            // Fire up the pulse.
            mpPulse = new QTimer(this);
            connect(mpPulse, &QTimer::timeout, [&]{
                Broadcast(Heartbeat_Packet, new QByteArray("~$$HEARTBEAT"));
            });
//            mpPulse->start(c_miPulseRate);
        }
        else
        {
            qCritical("Unable to bind to/listen on %s:%u!", sAddr.c_str(), uPort);
        }
    }
    else
    {
        qCritical("ERR: A service is currently using %s:%u!", sAddr.c_str(), uPort);
    }
    return bSuccess;
}

bool CServer::SendHandshake(u32 uClientUID)
{
    bool bSuccess = false;

    if (isListening())
    {
        QByteArray *pEncodedUID = EncodeClientUID(CW_NET_SERVERID);
        if (nullptr != pEncodedUID)
        {
            qInfo("Sending new client [%u] a handshake request...", uClientUID);
            bSuccess = Transmit(uClientUID, Handshake_Packet, pEncodedUID);
            delete pEncodedUID;
        }
    }
    else
    {
        qCritical("Unable to transmit on a null connection! Please set one up first!");
    }

    return bSuccess;
}

bool CServer::VerifyHandshake(SPacket lPacket, CTcpSocket* pClient)
{
    bool bSuccess = false;

    // This process is simple, we want to simply see if the client sent the correct encoded string (which should be their UID XOR'd with the 3rd byte of the packet ID ('W').
    // And if they did, we then check to make sure that they're not already registered.
    // If all is good, we respond with a HEARTBEAT, otherwise they're simply ignored and dropped.
    if (lPacket.mpPayload->toHex() == EncodeClientUID(lPacket.muClientUID)->toHex())
    {
        if (nullptr != pClient)
        {
            lPacket.muClientUID = pClient->GetUID();
        }

        if (mmClients.end() != mmClients.find(lPacket.muClientUID))
        {
            CTcpSocket* pClient = mmClients[lPacket.muClientUID];
            if (nullptr != pClient)
            {
                if (Handshake_State == pClient->GetState())
                {
                    bSuccess = Transmit(lPacket.muClientUID, Server_HostQuery, new QByteArray("~$$HOST_QUERY"));

                    if (!bSuccess)
                    {
                        qCritical("Unable to send HOST QUERY out to %s :(", pClient->GetIP().c_str());

                        mmClients.erase(lPacket.muClientUID);
                        pClient->SetState(Dead_State);
                        pClient->close();
                        delete pClient;
                    }
                    else
                    {
                        pClient->SetState(Verified_State);
                        qInfo("Successfully verified client: %s", pClient->GetIP().c_str());
                    }
                }
                else
                {
                    qCritical("Client (%s) tried to send a handshake packet before connecting! Dropping them!", pClient->GetIP().c_str());
                    pClient->close();
                    mmClients.erase(lPacket.muClientUID);
                    delete pClient;
                }
            }
            else
            {
                qCritical("Client is in the map, but they're null. Popping them...");
                mmClients.erase(lPacket.muClientUID);
            }
        }
        else
        {
            qCritical("Phantom client tried to handshake, ignoring...");
        }
    }
    else
    {
        qCritical("Received an invalid handshake packet, discarding...");
    }

    return bSuccess;
}

void CServer::Broadcast(EPacketType eType, QByteArray* pPayload)
{
    if (isListening())
    {
        for (std::map<u32, CTcpSocket*>::iterator pIter = mmClients.begin(); pIter != mmClients.end(); ++pIter)
        {
            std::pair<u32, CTcpSocket*> lTmp = (*pIter);
            Transmit(lTmp.first, eType, pPayload);
        }
    }
    else
    {
        qCritical("Unable to transmit on a null connection! Please set one up first!");
    }
}

bool CServer::Transmit(u32 uClient, EPacketType eType, QByteArray* pPayload)
{
    bool bSuccess = false;
    if (isListening())
    {
        if (mmClients.end() != mmClients.find(uClient))
        {
            CTcpSocket* pClient = mmClients[uClient];
            if (nullptr != pClient)
            {
                SPacket lPacket;
                lPacket.msID = "CwNp";
                lPacket.muClientUID = pClient->GetUID();
                lPacket.meType = eType;
                lPacket.mpPayload = pPayload;

                QByteArray *pPacket = CProtocol::CreatePacket(lPacket);
                if (nullptr != pPacket)
                {
                    bSuccess = (pPacket->length() <= pClient->WriteData(pPacket));
                    delete pPacket;
                }
            }
            else
            {
                qCritical("Client is null, popping them from the list.");
                mmClients.erase(uClient);
            }
        }
        else
        {
            qCritical("That client isn't connected to us. :/");
        }
    }
    else
    {
        qCritical("Unable to transmit on a null connection! Please set one up first!");
    }

    return bSuccess;
}

void CServer::FlushAll()
{
    if (isListening())
    {
        for (std::map<u32, CTcpSocket*>::iterator pIter = mmClients.begin(); pIter != mmClients.end(); ++pIter)
        {
            std::pair<u32, CTcpSocket*> lTmp = (*pIter);
            lTmp.second->ReadData();
            lTmp.second->flush();
        }
    }
}

void CServer::incomingConnection(qintptr socketDescriptor)
{
    CTcpSocket *pNewSock = new CTcpSocket(this);
    pNewSock->setSocketDescriptor(socketDescriptor);
    mqNewClients.push(pNewSock);
    emit newConnection();
}

void CServer::NewClient()
{
    if (!mqNewClients.empty())
    {
        CTcpSocket* pNewClient = mqNewClients.front();
        mqNewClients.pop();

        if (nullptr != pNewClient)
        {
            qInfo("New client: %s", pNewClient->GetIP().c_str());

            mmClients.insert(std::pair<u32, CTcpSocket*>(pNewClient->GetUID(), pNewClient));

            pNewClient->SetState(Unverified_State);
            connect(pNewClient, &CTcpSocket::readyRead, pNewClient, &CTcpSocket::ReadData);
            connect(pNewClient, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), pNewClient, &CTcpSocket::HandleError);
            connect(pNewClient, &CTcpSocket::DataInput, this, &CServer::HandleInput);

            SendHandshake(pNewClient->GetUID());
            pNewClient->SetState(Handshake_State);
        }
    }
}

void CServer::HandleInput(CTcpSocket* pClient, QByteArray* pData)
{
    if (nullptr != pData && CProtocol::IsValid(pData) && nullptr != pClient)
    {
        // Parse the input packet.
        SPacket lPacket = CProtocol::ParsePacket(pData);

        if (mmClients.end() != mmClients.find(pClient->GetUID()))
        {
            switch (lPacket.meType)
            {
                case Heartbeat_Packet:
                {
                    if (pClient->GetState() == Verified_State)
                    {
                        qDebug("Heartbeat: client --> server [GOOD] (%s)", pClient->GetIP().c_str());
                    }
                    break;
                }
                case Command_Packet:
                {
                    if (pClient->GetState() == Verified_State)
                    {
                        qInfo("Client (%s) sent a command: %s", pClient->GetIP().c_str(), lPacket.mpPayload->data());
                        emit SendCommand(ParseCommandString(QString::fromStdString(lPacket.mpPayload->toStdString()), pClient->GetHostname(), pClient->GetUID()));
                    }
                    break;
                }
                case Update_Packet:
                {
                    qWarning("Client %s [%s] is trying to update our board :/", pClient->GetIP().c_str(), pClient->GetHostname().c_str());
                    break;
                }
                case Log_Packet:
                {
                    qWarning("Client %s [%s] is trying to update our logs :/", pClient->GetIP().c_str(), pClient->GetHostname().c_str());
                    break;
                }
                case Handshake_Packet:
                {
                    if (pClient->GetState() == Handshake_State) { VerifyHandshake(lPacket, pClient); }
                    break;
                }
                case Client_HostResponse:
                {
                    if (pClient->GetState() == Verified_State)
                    {
                        pClient->SetHostname(lPacket.mpPayload->toStdString());

                        qInfo("Client (%s) said they're named: %s", pClient->GetIP().c_str(), pClient->GetHostname().c_str());

                        // Heartbeat back.
                        Transmit(pClient->GetUID(), Heartbeat_Packet, new QByteArray("~$$HEARTBEAT"));

                        emit NewClientVerified(pClient->GetUID()); // This allows an external system to be aware of the newly connected and verified client.
                    }
                    break;
                }
                default: /* case Unknown_Packet: */
                {
                    qCritical("Received unknown or invalid packet from %s [%s]! Ignoring...", pClient->GetIP().c_str(), pClient->GetHostname().c_str());
                    break;
                }
            }
        }
        else
        {
            qWarning("Phantom client from: %s, dropping.", pClient->GetIP().c_str());
            pClient->close();
            delete pClient;
            pClient = nullptr;
        }
    }
    else
    {
        qCritical("Received invalid packet data! Discarding...");
    }
}

QByteArray* CServer::EncodeClientUID(u32 uUID, byte bEncodeByte)
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
