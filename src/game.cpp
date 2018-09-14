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
    muLastRoll = (uMax > 0) ? (static_cast<u32>(rand()) % uMax) + uMin : static_cast<u32>(rand());
    return muLastRoll;
}

u32 CDice::GetLastRoll()
{
    return muLastRoll;
}
// ================================ End CDice Implementation ================================ //


// ================================ Begin CGame Implementation ================================ //
CGame::CGame() : mpDice{nullptr}, mpBoard{nullptr}, muDiceMax{0xffffffff}
{
    // Intentionally left blank.
}

CGame::CGame(const CGame& aCls) : mpDice{aCls.mpDice}, mpBoard{aCls.mpBoard}, muDiceMax{aCls.muDiceMax}
{
    // Intentionally left blank.
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
    }
    else
    {
        qCritical("ERR: Cannot run NewGame on a started game! Please end the current game first!");
    }
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
                        if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); }
                        else { qCritical(rtnData.second.toStdString().c_str()); }
//                    }
                }
                else
                {
                    // Move medium amount!
                    std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 6);
                    if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); }
                    else { qCritical(rtnData.second.toStdString().c_str()); }
                }
            }
            else
            {
                // Move small amount!
                std::pair<bool, QString> rtnData = MoveColor(eAggressor, eVictim, 3);
                if (rtnData.first) { qInfo(rtnData.second.toStdString().c_str()); }
                else { qCritical(rtnData.second.toStdString().c_str()); }
            }
        }

        // Check if there is only 1 color.
        if (1 == mvNations.size())
        {
            qInfo(QString("%1 has won!").arg(g_ColorNameMap[mvNations[0]->GetNationColor()]).toStdString().c_str());
            EndGame();
        }
    }
}

void CGame::EndGame()
{
    if (nullptr != mpDice) { delete mpDice; }
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

        if (nullptr != pAggrNation && nullptr != pVictimNation)
        {
            /* Attempt to make the move.
             *
             * In order to do this, we first acquire all the aggressor combs and iterate through them. First testing to see if one of them is shared with the victim.
             * IF TRUE: We then attempt to claim all the victim's cells (up to uMvAmnt).
             * IF FALSE: We then check the neighbors and see if one of them has the victim.
             *      IF TRUE: We then attack the new comb, and go from there.
             *      IF FALSE: We move on to the next comb
             *
             * If we don't get uMvAmnt of cells, we report back saying "true, however only X cells taken, no neighbors remain."
             */
            u32 uCellsTaken = 0;
            std::vector<CHoneyComb*> vAggrCombs = pAggrNation->GetAllCombs();
            for (std::vector<CHoneyComb*>::iterator pAggrIter = vAggrCombs.begin(); pAggrIter != vAggrCombs.end(); ++pAggrIter)
            {
                // Check to see if this comb contains the victim nation.
                CHoneyComb* pCurrComb = (*pAggrIter);
                if (nullptr != pCurrComb)
                {
                    if (pCurrComb->CombContainsColor(eVictim))
                    {
                        // It does! Take their cells!
                        do
                        {
                            u32 uCellIdx = pCurrComb->GetCellIdxColor(eVictim);
                            pAggrNation->Add(*pAggrIter, uCellIdx);
                            pVictimNation->Remove(*pAggrIter, uCellIdx);
                            ++uCellsTaken;

                            if (pCurrComb->CombIsAllColor(eAggressor)) { pCurrComb->SetCombColor(eAggressor); pVictimNation->Remove(*pAggrIter, uCellIdx); }
                            else { pCurrComb->SetCombColor(Comb_Mixed); }

                            if (uCellsTaken >= uMvAmnt) { break; }
                        } while ( pCurrComb->CombContainsColor(eVictim) && uCellsTaken < uMvAmnt);

                        if (uCellsTaken >= uMvAmnt) { break; }
                    }

                    // Grab the neighbors and iterate through them.
                    std::vector<CHoneyComb*> pAggrNeighbors = mpBoard->GetNeighbors(pCurrComb);
                    for (std::vector<CHoneyComb*>::iterator pNeighborIter = pAggrNeighbors.begin(); pNeighborIter != pAggrNeighbors.end(); ++pNeighborIter)
                    {
                        CHoneyComb* pNeighbor = (*pNeighborIter);
                        if (nullptr != pNeighbor)
                        {
                            if (pNeighbor->CombContainsColor(eVictim))
                            {
                                // It does! Take their cells!
                                do
                                {
                                    u32 uCellIdx = pNeighbor->GetCellIdxColor(eVictim);
                                    pAggrNation->Add(pNeighbor, uCellIdx);
                                    pVictimNation->Remove(pNeighbor, uCellIdx);
                                    ++uCellsTaken;

                                    if (pNeighbor->CombIsAllColor(eAggressor)) { pNeighbor->SetCombColor(eAggressor); pVictimNation->Remove(pNeighbor, uCellIdx); }
                                    else { pNeighbor->SetCombColor(Comb_Mixed); }

                                    if (uCellsTaken >= uMvAmnt) { break; }
                                } while ( pNeighbor->CombContainsColor(eVictim) && uCellsTaken < uMvAmnt);
                            }
                        }

                        if (uCellsTaken >= uMvAmnt) { break; }
                    }
                }

                if (uCellsTaken >= uMvAmnt) { break; }
            }

            // Have we conquered?
            if (0 >= pVictimNation->GetNationSize())
            {
                mvNations.erase(std::find(mvNations.begin(), mvNations.end(), pVictimNation));
                QString sAggrName = pAggrNation->GetNationName();
                QString sVictimName = pVictimNation->GetNationName();
                QString sOvertakeMsg = QString("%1 has conquered %2!").arg(sAggrName, sVictimName);
                rtnData = std::pair<bool, QString>(true, sOvertakeMsg);
            }
            else
            {
                rtnData = std::pair<bool, QString>(true, QString("%1 Took %2 cells!").arg(pAggrNation->GetNationName()).arg(uCellsTaken));
            }
        }
        else if (nullptr != pAggrNation && nullptr == pVictimNation) { rtnData = std::pair<bool, QString>(false, "Their nation doesn't exist!"); }
        else if (nullptr == pAggrNation && nullptr != pVictimNation) { rtnData = std::pair<bool, QString>(false, "Your nation doesn't exist!"); }
        else { rtnData = std::pair<bool, QString>(false, "Neither nation exists!"); }
    }

    return rtnData;
}

void CGame::Draw(QPainter *pPainter)
{
    if (nullptr != mpBoard)
    {
        mpBoard->Draw(pPainter);
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
        if ((*pNatIter)->GetNationColor() == eColor)
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

void CGame::SetDiceMax(u32 iMaxium)
{
    muDiceMax = iMaxium;
}
// ================================ End CGame Implementation ================================ //
