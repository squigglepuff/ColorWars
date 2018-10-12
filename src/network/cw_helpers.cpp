#include "include/network/cw_helpers.h"

#if !defined(_EXTERN_CMDPARSE_FXN)
SCommand ParseCommandString(QString) { return SCommand(); }
#endif // #if !defined(_EXTERN_CMDPARSE_FXN)

QByteArray* PackBoardMap(std::map<u64, ECellColors> mData)
{
    QByteArray* pRtn = new QByteArray();

    for (std::map<u64, ECellColors>::iterator pIter = mData.begin(); pIter != mData.end(); ++pIter)
    {
        std::pair<u64, ECellColors> lTmpPair = (*pIter);

        const size_t c_iBuffMx = 32;
        char* pTmpBuff = new char[c_iBuffMx];
        memset(pTmpBuff, 0, c_iBuffMx);

        snprintf(pTmpBuff, c_iBuffMx, "%lld:", lTmpPair.first);

        pRtn->append(pTmpBuff);

        u16 uColor = static_cast<u16>(lTmpPair.second);
        memset(pTmpBuff, 0, c_iBuffMx);

        snprintf(pTmpBuff, c_iBuffMx, "%u,", uColor);

        pRtn->append(pTmpBuff);

        if (nullptr != pTmpBuff) { delete[] pTmpBuff; }
    }

    return pRtn;
}

std::map<u64, ECellColors> UnpackBoardMap(QByteArray* pData)
{
    std::map<u64, ECellColors> mRtn;

    QByteArray lKey = "";
    QByteArray lClr = "";
    bool bInKey = true;
    while (!pData->isEmpty())
    {
        char cTmp = pData->at(0);

        if (cTmp == ':')
        {
            bInKey = false;
        }
        else if (cTmp == ',')
        {
            mRtn.insert(std::pair<u64, ECellColors>(lKey.toULongLong(), static_cast<ECellColors>(lClr.toUShort())));
            lKey.clear();
            lClr.clear();
            bInKey = true;
        }
        else if ('0' <= cTmp && '9' >= cTmp)
        {
            if (bInKey) { lKey.append(cTmp); }
            else { lClr.append(cTmp); }
        }
        else
        {
            qCritical("Found non-digit char '%c' in color map! Discarding...", cTmp);
        }

        pData->remove(0, 1); // Remove a single char.
    }

    return mRtn;
}
