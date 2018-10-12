#include "include/network/cw_protocol.h"

CProtocol::CProtocol()
{
    // Intentionally left blank.
}

CProtocol::~CProtocol()
{
    // Intentionally left blank.
}

QByteArray* CProtocol::CreatePacket(SPacket lPacket)
{
    QByteArray *pRtn = new QByteArray(lPacket.msID.c_str());

    const size_t ciBuffSz = 16;
    char *pTmpBuff = new char[ciBuffSz];

    // First the UID.
    memset(pTmpBuff, 0, ciBuffSz);
    snprintf(pTmpBuff, 8, "%c%c%c%c", ((lPacket.muClientUID & 0xff000000) >> 24), ((lPacket.muClientUID & 0x00ff0000) >> 16), ((lPacket.muClientUID & 0x0000ff00) >> 8), (lPacket.muClientUID & 0x000000ff));
    pRtn->append(pTmpBuff);

    // Next the packet type.
    u16 uPacketType = static_cast<u16>(lPacket.meType);
    memset(pTmpBuff, 0, ciBuffSz);
    snprintf(pTmpBuff, 8, "%c%c", ((uPacketType & 0xff00) >> 8), (uPacketType & 0x00ff));
    pRtn->append(pTmpBuff);

    // Now the payload.
    pRtn->append(*(lPacket.mpPayload));

    // Obfuscate the data.
    pRtn = XorData(pRtn);

    // Append the ETX and EOT bytes.
    pRtn->append(ProtocolEOF());

    // DONE!
    if (nullptr != pTmpBuff) { delete[] pTmpBuff; }

    return pRtn;
}

SPacket CProtocol::ParsePacket(QByteArray* pData)
{
    SPacket lRtn = SPacket();

    if (nullptr != pData && IsValid(pData))
    {
        if (pData->startsWith(XorData(new QByteArray("CwNp"))->toStdString().c_str()))
        {
            pData = XorData(pData);
        }

        lRtn.msID = pData->left(4).toStdString();
        pData->remove(0, 4);

        QByteArray lClientUID = pData->left(sizeof(u32));
        pData->remove(0, sizeof(u32));

        lRtn.muClientUID = 0x00000000;
        lRtn.muClientUID |= static_cast<byte>(lClientUID[0]) << 24;
        lRtn.muClientUID |= static_cast<byte>(lClientUID[1]) << 16;
        lRtn.muClientUID |= static_cast<byte>(lClientUID[2]) << 8;
        lRtn.muClientUID |= static_cast<byte>(lClientUID[3]);

        QByteArray lPacketType = pData->left(sizeof(u16));
        pData->remove(0, sizeof(u16));

        u16 uTypeID = 0x0000;
        uTypeID |= static_cast<byte>(lPacketType[0]) << 8;
        uTypeID |= static_cast<byte>(lPacketType[1]);
        lRtn.meType = static_cast<EPacketType>(uTypeID);

        lRtn.mpPayload = new QByteArray(*pData);

        if (nullptr != pData) { delete pData; }
    }

    return lRtn;
}

bool CProtocol::IsValid(QByteArray* pPacket)
{
    const char* pObfStr = XorData(new QByteArray("CwNp"))->toStdString().c_str();
    return (nullptr != pPacket && (pPacket->startsWith("CwNp") || pPacket->startsWith(pObfStr)));
}

bool CProtocol::IsValid(SPacket lPacket)
{
    return (0 == lPacket.msID.compare("CwNp"));
}

QByteArray* CProtocol::XorData(QByteArray* pData)
{
    if (nullptr != pData)
    {
#if 0
        for (int iIdx = 0; iIdx < pData->size(); ++iIdx)
        {
            pData->replace(pData->at(iIdx), static_cast<char>(pData->at(iIdx) ^ 0x1b));
        }
#else
        char* pRtn = pData->data();
        for (char* pIter = pRtn; nullptr != pIter && NULL != (*pIter); ++pIter)
        {
            (*pIter) = (*pIter) ^ 0x1b;
        }
#endif
    }

    return pData;
}

const char* CProtocol::ProtocolEOF()
{
    return "\x03\x04";
}
