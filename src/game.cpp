#include "include/game.h"

// ================================ Begin CDice Implementation ================================ //
CDice::CDice() : muLastRoll{0}
{
    // Seed the RNG.
    srand(static_cast<u32>(clock()));
}

CDice::CDice(const CDice&aCls) : muLastRoll{aCls.muLastRoll}
{
    // Intentionally left blank.
}

CDice::~CDice()
{
    // Intentionally left blank.
}

CDice& CDice::operator=(const CDice& aCls)
{
    muLastRoll = aCls.muLastRoll;
    return *this;
}

u32 CDice::Roll(u32 uMin, u32 uMax)
{
    u32 uRoll = floor(rand());
    if (uRoll > uMax)
    {
        muLastRoll = (uMax > 0) ? (uRoll % uMax) + uMin : static_cast<u32>(uRoll);
    }
    else
    {
        muLastRoll = (uMax > 0) ? (uMax % uRoll) + uMin : static_cast<u32>(uRoll);
    }

    return muLastRoll;
}

u32 CDice::GetLastRoll()
{
    return muLastRoll;
}
// ================================ End CDice Implementation ================================ //


// ================================ Begin CGame Implementation ================================ //
CGame::CGame() : mpDice{nullptr}, mpBoard{nullptr}, mpCanvas{nullptr}, muDiceMax{0xffffffff}, msTmpFileName{"colorwars_development.png"}
{
    if (msTmpFileName.find("_DEBUG") == std::string::npos && g_cfgVars.mbIsDebug)
    {
        msTmpFileName.insert(msTmpFileName.find_last_of("."), "_DEBUG");
    }
}

CGame::CGame(const CGame& aCls) : mpDice{aCls.mpDice}, mpBoard{aCls.mpBoard}, mpCanvas{aCls.mpCanvas}, muDiceMax{aCls.muDiceMax}
{
    if (msTmpFileName.find("_DEBUG") == std::string::npos && g_cfgVars.mbIsDebug)
    {
        msTmpFileName.insert(msTmpFileName.find_last_of("."), "_DEBUG");
    }
}

CGame::~CGame()
{
    if (nullptr != mpDice) { delete mpDice; }
    if (nullptr != mpBoard)
    {
        mpBoard->Destroy();
    }
}

CGame& CGame::operator=(const CGame& aCls)
{
    if (this != &aCls)
    {
        mpDice = aCls.mpDice;
        mpBoard = aCls.mpBoard;
        muDiceMax = aCls.muDiceMax;
    }

    return *this;
}

void CGame::NewGame(u32 iDiceMax, u32 uCellSz, SPoint qCenter)
{
    if (nullptr == mpDice && nullptr == mpBoard)
    {
        muDiceMax = iDiceMax;

        // Always run clean!
        EndGame();

        // Instantiate a new CDice object.
        mpDice = new CDice();

        // Instantiate a new board.
        mpBoard = new CBoard();
        mpBoard->Create(uCellSz, qCenter);
        mvNations = mpBoard->GetNationList();

        // Update the canvas.
        int iCanvasSz = static_cast<int>(qCenter.y() * 2);
        mpCanvas = new QImage(iCanvasSz, iCanvasSz, QImage::Format_ARGB32);
        mpCanvas->fill(Qt::transparent); // Fills the canvas with transparency.

        // Update!
        Draw();
    }
    else
    {
        qCritical("ERR: Cannot run NewGame on a started game! Please end the current game first!");
    }
}

void CGame::EndGame()
{
//    if (nullptr != mpCanvas && !mpCanvas->isNull()) { delete mpCanvas; }
}

void CGame::Play(ECellColors eAggressor, ECellColors eVictim)
{
    if (nullptr != mpDice && nullptr != mpBoard)
    {
        /* Determine the ranges we need to be in for movement amounts.
         * These are:
         *  [x - y](25% of range){center of range}  --->  Move 3 spaces
         *  [x - y](10% of range){center of range}  --->  Move 6 spaces
         *  [x - y](5% of range){center of range}   --->  Move 12 spaces
         *  [x - y](1% of range){center of range}   --->  Overtake
         */
        const u32 uSmallMvRange = static_cast<u32>(muDiceMax * 0.25f);
        const u32 uMedMvRange = static_cast<u32>(muDiceMax * 0.10f);
        const u32 uLrgMvRange = static_cast<u32>(muDiceMax * 0.05f);
//        const u32 uHugeMvRange = static_cast<u32>(muDiceMax * 0.01f);
        const u32 uMidOfRange = static_cast<u32>(muDiceMax / 2.0f);

        // Roll the dice!
        u32 uRoll = mpDice->Roll(3, muDiceMax);

        // Check to see if we should move.
        if ((uSmallMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uSmallMvRange) <= uRoll)
        {
            if ((uMedMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uMedMvRange) <= uRoll)
            {
                if ((uLrgMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uLrgMvRange) <= uRoll)
                {
                    /*if ((uHugeMvRange + uMidOfRange) >= uRoll && (uMidOfRange - uHugeMvRange) <= uRoll)
                    {
                        // Overtake! Move HUGE amount!
                        std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 0);
                        if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); }
                        else { qCritical(rtnData.second.toStdString().c_str()); }
                    }
                    else
                    {*/
                        // Move large amount!
                        std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 12);
                        if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); Draw(); }
                        else { qCritical(rtnData.second.toStdString().c_str()); }
//                    }
                }
                else
                {
                    // Move medium amount!
                    std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 6);
                    if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); Draw(); }
                    else { qCritical(rtnData.second.toStdString().c_str()); }
                }
            }
            else
            {
                // Move small amount!
                std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 3);
                if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); Draw(); }
                else { qCritical(rtnData.second.toStdString().c_str()); }
            }
        }

        // Check if there is only 1 color.
        if (1 == mvNations.size())
        {
            qInfo(QString("%1 has won!").arg(g_ColorNameMap[mvNations[0]->GetNationColor()]).toStdString().c_str());
            Draw();
//            EndGame();
        }
    }
}

void CGame::Destroy()
{
    // Delete the dice and board.
    if (nullptr != mpDice) { delete mpDice; }
    if (nullptr != mpBoard)
    {
        mpBoard->Destroy();
        delete mpBoard;
    }
}

std::pair<bool, QString> CGame::MoveColor(ECellColors eAggressor, ECellColors eVictim, u32 uMvAmnt)
{
    std::pair<bool, QString> rtnData = std::pair<bool, QString>(false, "No move made!");
    if (nullptr != mpDice && nullptr != mpBoard)
    {
        // Check to see if these colors currently have live nations.
        CNation* pAggrNation = nullptr;
        CNation* pVictimNation = nullptr;
        for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
        {
            CNation *pTmpNat = (*pNatIter);
            if (nullptr != pTmpNat)
            {
                if (pTmpNat->GetNationColor() == eAggressor)
                {
                    pAggrNation = pTmpNat;
                }
                else if (pTmpNat->GetNationColor() == eVictim)
                {
                    pVictimNation = pTmpNat;
                }
            }
        }

        u32 uCellTaken = 0;
        QString lRtnStr = "No cells taken!";
        if (nullptr != pAggrNation && nullptr != pVictimNation)
        {
            uCellTaken = DoFloodFill(pAggrNation, pVictimNation, uMvAmnt);

            if (0 < uCellTaken)
            {
                lRtnStr = QString("%1 Took %2 cells from %3!").arg(pAggrNation->GetNationName()).arg(uCellTaken).arg(pVictimNation->GetNationName());
                if (0 >= pVictimNation->GetNationSize())
                {
                    mvNations.erase(std::find(mvNations.begin(), mvNations.end(), pVictimNation));
                    lRtnStr = QString("%1 has conquered %2!").arg(pAggrNation->GetNationName()).arg(pVictimNation->GetNationName());
                }
            }
            else
            {
                lRtnStr = QString("%1 doesn't border %2!").arg(pAggrNation->GetNationName()).arg(pVictimNation->GetNationName());
            }
        }
        else if (nullptr == pAggrNation && nullptr != pVictimNation)
        {
            lRtnStr = "Your nation doesn't exist!";
        }
        else if (nullptr != pAggrNation && nullptr == pVictimNation)
        {
            lRtnStr = "Their nation doesn't exist!";
        }
        else
        {
            lRtnStr = "Neither nation exists!";
        }

        rtnData = std::pair<bool, QString>(true, lRtnStr);
//        rtnData = DoInfectionFill(pAggrNation, pVictimNation, uMvAmnt);
    }

    return rtnData;
}

void CGame::Draw()
{
    if (nullptr != mpBoard)
    {
        QPainter *pPainter = new QPainter();
        pPainter->begin(mpCanvas);

        pPainter->setBackgroundMode(Qt::TransparentMode);
        pPainter->setBackground(Qt::transparent);

        mpBoard->Draw(pPainter);
        pPainter->end();

        if (nullptr != pPainter) { delete pPainter; }

        // Save the image.
        mpCanvas->save(QString::fromStdString(msTmpFileName));
    }
}

u32 CGame::DummyRoll()
{
    return (nullptr != mpDice) ? mpDice->Roll(3, muDiceMax) : 0;
}

bool CGame::NationExists(ECellColors eColor)
{
    bool bFoundNation = false;
    for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
    {
        if ((*pNatIter)->GetNationColor() == eColor && 0 < (*pNatIter)->GetNationSize())
        {
            bFoundNation = true;
            break;
        }
    }

    return bFoundNation;
}

u32 CGame::GetDiceMax()
{
    return muDiceMax;
}

CBoard* CGame::GetBoard()
{
    return mpBoard;
}

QImage* CGame::GetCanvas()
{
    return mpCanvas;
}

void CGame::SetDiceMax(u32 iMaxium)
{
    muDiceMax = iMaxium;
}

u32 CGame::DoFloodFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt)
{
    u32 uCellsTaken = 0;
    u32 uNewCells = 0;
    if (nullptr != aAggrNation && nullptr != aVictimNation)
    {
        // Get the nation colors.
        ECellColors eAggressor = aAggrNation->GetNationColor();
        ECellColors eVictim = aVictimNation->GetNationColor();

        do
        {
            uNewCells = 0;

            // Iterate over the aggressor's cells.
            std::vector<u64> vAggrCells = aAggrNation->GetCellIDs();
            for (std::vector<u64>::iterator iAggrCellIter = vAggrCells.begin(); iAggrCellIter != vAggrCells.end(); ++iAggrCellIter)
            {
                u64 uCellID = (*iAggrCellIter);

                // Grab the neighbors of the cell.
                std::vector<CCell*> vNeighbors = mpBoard->GetCellNeighbors(uCellID);

                QString lMsg("Got %1 neighbors");
                lMsg = lMsg.arg(vNeighbors.size());
                qDebug(lMsg.toStdString().c_str());

                // Iterate over the owned cells and check the neighbors.
                for (std::vector<CCell*>::iterator iNeighIter = vNeighbors.begin(); iNeighIter != vNeighbors.end(); ++iNeighIter)
                {
                    CCell* pCell = (*iNeighIter);
                    if (nullptr != pCell && pCell->GetColor() == eVictim)
                    {
                        // Yoink!
                        u64 uNewCellID = static_cast<u32>(pCell->GetPosition().mX);
                        uNewCellID = uNewCellID << 32;
                        uNewCellID += static_cast<u32>(pCell->GetPosition().mY);

                        QString qMsg("Taking cell %1 from %2 nation...");
                        qMsg = qMsg.arg(uNewCellID).arg(g_ColorNameMap[eVictim]);
                        qDebug(qMsg.toStdString().c_str());

                        pCell->SetColor(eAggressor);

                        aAggrNation->Add(uNewCellID);
                        aVictimNation->Remove(uNewCellID);
                        ++uCellsTaken;
                        uNewCells = 1;

                        if (uCellsTaken >= uMvAmnt) { break; }
                    }

                    if (uCellsTaken >= uMvAmnt) { break; }
                }

                if (uCellsTaken >= uMvAmnt) { break; }
            }
        } while(uCellsTaken < uMvAmnt && 0 < uNewCells);
    }

    return uCellsTaken;
}

u32 CGame::DoInfectionFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt)
{
    u32 uCellsTaken = 0;

    if (nullptr != aAggrNation && nullptr != aVictimNation)
    {
        // code
    }

    return uCellsTaken;
}
// ================================ End CGame Implementation ================================ //
