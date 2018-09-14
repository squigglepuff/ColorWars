#include "include/nation.h"

CNation::CNation() : meColor{Cell_White}, msName{"White"}
{
    // Intentionally left blank.
}

CNation::CNation(const CNation& aCls) : mvOwnedCells{aCls.mvOwnedCells}, meColor{aCls.meColor}, msName{aCls.msName}
{
    // Intentionally left blank.
}

CNation::~CNation()
{
    Destroy();
}

CNation& CNation::operator=(const CNation& aCls)
{
    if (this != &aCls)
    {
        mvOwnedCells = aCls.mvOwnedCells;
        meColor = aCls.meColor;
        msName = aCls.msName;
    }

    return *this;
}

CNation* CNation::operator <<(CNation& aChild)
{
    return aChild.Merge(this);
}

CNation* CNation::operator >>(CNation& aParent)
{
    return Merge(&aParent);
}

void CNation::Create(ECellColors eClr, CHoneyComb* pStartComb, QString sName)
{
    if (nullptr != pStartComb)
    {
        meColor = eClr;
        msName = sName;

        mvOwnedCells[pStartComb] = {0,1,2,3,4,5,6}; // We own all these cells!

        // Set the comb color!
        pStartComb->SetAllCellColor(eClr);
    }
}

void CNation::Destroy()
{
    // Simply purge the lists, do NOT delete anything as we do NOT own it!
    mvOwnedCells.clear();
}

bool CNation::Add(CHoneyComb *pComb, u32 iCellIdx)
{
    bool bSuccess = false;
    if (nullptr != pComb)
    {
        if (mvOwnedCells.end() == mvOwnedCells.find(pComb))
        {
            mvOwnedCells.insert(std::pair<CHoneyComb*, std::vector<u32>>(pComb, {iCellIdx}));
        }
        else
        {
            mvOwnedCells.at(pComb).push_back(iCellIdx);
        }

        // Set the color of the cell in question.
        pComb->GetCellAt(iCellIdx)->SetColor(meColor);

        bSuccess = true;
    }

    return bSuccess;
}

bool CNation::Remove(CHoneyComb *pComb, u32 iCellIdx)
{
    bool bSuccess = false;
    if (nullptr != pComb && 0 < GetNationSize())
    {
        // Check to see if we even own this comb via it's color, then via our tracking.
        if (pComb->GetCombColor() == meColor || pComb->GetCombColor() == Comb_Mixed)
        {
            std::map<CHoneyComb*, std::vector<u32>>::iterator pMapIter;
            for (pMapIter = mvOwnedCells.begin(); pMapIter != mvOwnedCells.end(); ++pMapIter)
            {
                if (pMapIter->first == pComb)
                {
                    for (std::vector<u32>::iterator pIdxIter = pMapIter->second.begin(); pIdxIter != pMapIter->second.end(); ++pIdxIter)
                    {
                        if (*pIdxIter == iCellIdx)
                        {
                            // Remove this cell from our tracking.
                            if (pMapIter->second.end() == pMapIter->second.erase(pIdxIter))
                            {
                                // We don't, we need to also remove the comb!
                                if (1 <= mvOwnedCells.size()) { mvOwnedCells.erase(pComb); }
                                else { mvOwnedCells.clear(); }
                            }

                            bSuccess = true;
                            break;
                        }

                        if (bSuccess) { break; }
                    }
                }

                if (bSuccess) { break; }
            }
        }
        else
        {
            // Remove this comb as it's not ours anymore!
            if (1 <= mvOwnedCells.size()) { mvOwnedCells.erase(pComb); }
            else { mvOwnedCells.clear(); }
            bSuccess = true;
        }
    }

    return bSuccess;
}

CNation* CNation::Merge(CNation* pMother)
{
    std::map<CHoneyComb*, std::vector<u32>>::iterator pMapIter;
    for (pMapIter = mvOwnedCells.begin(); pMapIter != mvOwnedCells.end(); ++pMapIter)
    {
        if (pMapIter != mvOwnedCells.end())
        {
            for (std::vector<u32>::iterator pIdxIter = pMapIter->second.begin(); pIdxIter != pMapIter->second.end(); ++pIdxIter)
            {
                pMother->Add(pMapIter->first, *pIdxIter);
            }
        }
    }

    // We don't own them anymore!
    mvOwnedCells.clear();

    // Done! Return the new mother!
    return pMother;
}

std::vector<CHoneyComb*> CNation::GetAllCombs()
{
    std::vector<CHoneyComb*> mRtn;
    std::map<CHoneyComb*, std::vector<u32>>::iterator pMapIter;
    for (pMapIter = mvOwnedCells.begin(); pMapIter != mvOwnedCells.end(); ++pMapIter)
    {
        mRtn.push_back(pMapIter->first);
    }
    return mRtn;
}

u32 CNation::GetNationSize()
{
    return mvOwnedCells.size();
}

ECellColors CNation::GetNationColor()
{
    return meColor;
}

QString CNation::GetNationName()
{
    return msName;
}

void CNation::SetNationColor(ECellColors eColor)
{
    if (Comb_Mixed != eColor)
    {
        meColor = eColor;
    }
}

void CNation::SetNationName(QString sName)
{
    if (!sName.isEmpty())
    {
        msName = sName;
    }
}
