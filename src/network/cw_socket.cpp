#include "include/network/cw_socket.h"

// -------------------------------- BEGIN CUdpSocket -------------------------------- //
CTcpSocket::CTcpSocket(QObject *pParent) : QTcpSocket{pParent}, meState{Unverified_State}, msHostname{""}
{
    // Intentionally left blank.
}
CTcpSocket::~CTcpSocket()
{
    // Intentionally left blank.
}

bool CTcpSocket::Connect(std::string sAddr, u16 uPort)
{
    connectToHost(QString::fromStdString(sAddr), uPort, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    bool bSuccess = isOpen();

    if (bSuccess)
    {
        // Connect up the QMetaObject calls.
        connect(this, &QAbstractSocket::readyRead, this, &CTcpSocket::ReadData);
        connect(this, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &CTcpSocket::HandleError);
    }

    return bSuccess;
}

// Getters.
u32 CTcpSocket::GetUID()
{
    u32 uPorts = (peerPort() << 16) ^ localPort();
    u32 uRemoteIP = peerAddress().toIPv4Address();
    u32 uUID = (uPorts ^ uRemoteIP);
    return uUID;
}

u16 CTcpSocket::GetPort()
{
    return localPort();
}

std::string CTcpSocket::GetIP()
{
    return localAddress().toString().toStdString();
}

std::string CTcpSocket::GetHostname()
{
    if (msHostname.empty()) { msHostname = QHostInfo::localHostName().toStdString(); }
    return msHostname;
}

ESocketState CTcpSocket::GetState()
{
    return meState;
}

void CTcpSocket::SetHostname(std::string sHostname)
{
    if (sHostname.empty())
    {
        msHostname = QHostInfo::localHostName().toStdString();
    }
    else
    {
        msHostname = sHostname;
    }
}

void CTcpSocket::ReadData()
{
    QByteArray *pData = new QByteArray();

    bool bKeepReading = true;
    bool bFoundETX = false;
    while (bKeepReading)
    {
        char cChar = '\0';
        if (getChar(&cChar))
        {
            if (bFoundETX && cChar == CProtocol::ProtocolEOF()[1])
            {
                bKeepReading = false;
                break;
            }
            else
            {
                if (cChar == CProtocol::ProtocolEOF()[0])
                {
                    bFoundETX = true;
                }
                else
                {
                    bFoundETX = false;
                    pData->append(cChar);
                }
            }
        }
        else
        {
            bKeepReading = false;
            break;
        }
    }

    if (!pData->isEmpty()) { emit DataInput(this, pData); }
}

u64 CTcpSocket::WriteData(QByteArray *pData)
{
    u64 uBytesWritten = 0;
    if (nullptr != pData && isWritable())
    {
        uBytesWritten = writeData(pData->data(), pData->length());
        flush();
    }

    return uBytesWritten;
}

void CTcpSocket::HandleError(QAbstractSocket::SocketError sockErr)
{
    std::string sRemoteIP = peerAddress().toString().toStdString();
    switch (sockErr)
    {
        case QAbstractSocket::ConnectionRefusedError:
        {
            qCritical("Unable to connect to %s:%u! [ERR_CONN_REFUSED]", sRemoteIP.c_str(), peerPort());
            break;
        }
        case QAbstractSocket::RemoteHostClosedError:
        {
            qCritical("%s closed the connection!", sRemoteIP.c_str());
            break;
        }
        case QAbstractSocket::HostNotFoundError:
        {
            qCritical("%s not found! [ERR_HOST_NOT_FOUND]", sRemoteIP.c_str());
            break;
        }
        case QAbstractSocket::SocketAccessError:
        {
            qCritical("Unable to access allocated socket!");
            break;
        }
        case QAbstractSocket::SocketResourceError:
        {
            qCritical("Unable to allocate socket due to resources!");
            break;
        }
        case QAbstractSocket::SocketTimeoutError:
        {
            qCritical("Socket has timed out on an operation!");
            break;
        }
        case QAbstractSocket::DatagramTooLargeError:
        {
            qCritical("Input/Output data is too big! [ERR_DAT_TOO_LRG]");
            break;
        }
        case QAbstractSocket::AddressInUseError:
        {
            qCritical("Cannot bind to %s:%u, someone is using it! [ERR_ADDR_IN_USE]", sRemoteIP.c_str(), peerPort());
            break;
        }
        case QAbstractSocket::SocketAddressNotAvailableError:
        {
            qCritical("%s isn't available or is invalid!", sRemoteIP.c_str());
            break;
        }
        case QAbstractSocket::UnsupportedSocketOperationError:
        {
            qCritical("Specified operation not supported!");
            break;
        }
        case QAbstractSocket::UnfinishedSocketOperationError:
        case QAbstractSocket::OperationError:
        {
            qCritical("Socket closed before operation could complete!");
            break;
        }
        case QAbstractSocket::TemporaryError:
        {
            qCritical("Socket temporarily unavailable!");
            break;
        }
        default:
        {
            qCritical("An unknown socket error has occured!");
            break;
        }
    }
}

void CTcpSocket::SetState(ESocketState eState)
{
    meState = eState;
}
// -------------------------------- END CUdpSocket -------------------------------- //
