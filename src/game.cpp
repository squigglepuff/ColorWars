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
        delete mpBoard;
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

void CGame::NewGame(u32 iDiceMax, u32 uCellSz, QPointF qCenter)
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
        mpNations = mpBoard->GetNationList();
    }
    else
    {
        qCritical("ERR: Cannot run NewGame on a started game! Please end the current game first!");
    }
}

void CGame::EndGame()
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
        for (std::vector<CNation*>::iterator pNatIter = *(mpNations->end()); pNatIter != *(mpNations->begin()); --pNatIter)
        {
            if ((*pNatIter)->GetNationColor() == eAggressor)
            {
                pAggrNation = (*pIter);
            }
            else if ((*pNatIter)->GetNationColor() == eVictim)
            {
                pVictimNation = (*pIter);
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
                if ((*pAggrIter)->CombContainsColor(eVictim))
                {
                    // It does! Take their cells!
                    do
                    {
                        u32 uCellIdx = (*pAggrIter)->GetCellIdxColor(eVictim);
                        pAggrNation->Add(*pAggrIter, uCellIdx);
                        pVictimNation->Remove(*pAggrIter, uCellIdx);
                        ++uCellsTaken;

                        if (uCellsTaken >= uMvAmnt) { break; }
                    } while ( (*pAggrIter)->GetCellIsColor(eVictim) && uCellsTaken < uMvAmnt);

                    if (uCellsTaken >= uMvAmnt) { break; }
                }

                // Grab the neighbors and iterate through them.
                for (CHoneyComb** pAggrNeighbors = mpBoard->GetNeighbors((*pAggrIter)); nullptr != pAggrNeighbors; ++pAggrNeighbors)
                {
                    if ((*pAggrNeighbors)->CombContainsColor(eVictim))
                    {
                        // It does! Take their cells!
                        do
                        {
                            u32 uCellIdx = (*pAggrIter)->GetCellIdxColor(eVictim);
                            pAggrNation->Add(*pAggrIter, uCellIdx);
                            pVictimNation->Remove(*pAggrIter, uCellIdx);
                            ++uCellsTaken;

                            if (uCellsTaken >= uMvAmnt) { break; }
                        } while ( (*pAggrIter)->GetCellIsColor(eVictim) && uCellsTaken < uMvAmnt);
                    }

                    if (uCellsTaken >= uMvAmnt) { break; }
                }

                if (uCellsTaken >= uMvAmnt) { break; }
            }

            rtnData = std::pair<bool, QString>(false, tr("Took %1 cells!").arg(uCellsTaken));
        }
        else if (nullptr != pAggrNation && nullptr == pVictimNation)
        {
            rtnData = std::pair<bool, QString>(false, "Their nation doesn't exist!");
        }
        else if (nullptr == pAggrNation && nullptr != pVictimNation)
        {
            rtnData = std::pair<bool, QString>(false, "Your nation doesn't exist!");
        }
        else
        {
            rtnData = std::pair<bool, QString>(false, "Neither nation doesn't exist!");
        }
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
