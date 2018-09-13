#include "include/nation.h"

CNation::CNation() : meColor{Cell_White}, msName{"White"}
{
    // Intentionally left blank.
}

CNation::CNation(const CNation& aCls) : mvOwnedCombs{aCls.mvOwnedCombs}, mvOwnedCells{aCls.mvOwnedCells}, meColor{aCls.meColor}, msName{aCls.msName}
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
        mvOwnedCombs = aCls.mvOwnedCombs;
        mvOwnedCells = aCls.mvOwnedCells;
        meColor = aCls.meColor;
        msName = aCls.msName;
    }

    return *this;
}

CNation& CNation::operator <<(CNation& aChild)
{
    return aChild.Merge(*this);
}

CNation& CNation::operator >>(CNation& aParent)
{
    return Merge(aParent);
}

void CNation::Create(ECellColors eClr, CHoneyComb* pStartComb, QString sName)
{
    if (nullptr != pStartComb)
    {
        meColor = eClr;
        msName = sName;

        mvOwnedCombs.push_back(pStartComb);
        mvOwnedCells[pStartComb] = {0,1,2,3,4,5,6}; // We own all these cells!

        // Set the comb color!
        pStartComb->SetCombColor(eClr);
    }
}

void CNation::Destroy()
{
    // Simply purge the lists, do NOT delete anything as we do NOT own it!
    mvOwnedCombs.clear();
    mvOwnedCells.clear();
}

bool CNation::Add(CHoneyComb *pComb, u32 iCellIdx)
{
    bool bSuccess = false;
    if (nullptr != pComb)
    {
        if (std::find(mvOwnedCombs.begin(), mvOwnedCombs.end(), pComb) == mvOwnedCombs.end())
        {
            mvOwnedCombs.push_back(pComb);
        }

        if (mvOwnedCells.end() == mvOwnedCells.find(pComb))
        {
            mvOwnedCells.insert(std::pair<CHoneyComb*, std::vector<u32>>(pComb, {iCellIdx}));
        }
        else
        {
            mvOwnedCells.at(pComb).push_back(iCellIdx);
        }

        // Set the color of the cell in question.
        for (std::vector<u32>::iterator pCellIter = mvOwnedCells.at(pComb).begin(); pCellIter != mvOwnedCells.at(pComb).end(); ++pCellIter)
        {
            pComb->GetCellAt((*pCellIter))->SetColor(meColor);
        }

        bSuccess = true;
    }

    return bSuccess;
}

bool CNation::Remove(CHoneyComb *pComb, u32 iCellIdx)
{
    bool bSuccess = false;
    if (nullptr != pComb)
    {
        std::vector<CHoneyComb*>::iterator pCombIter = std::find(mvOwnedCombs.begin(), mvOwnedCombs.end(), pComb);
        if (pCombIter != mvOwnedCombs.end())
        {
            // Check to see if we even own any of the cells.
            if (mvOwnedCells.find(pComb) != mvOwnedCells.end())
            {
                std::vector<u32>::iterator pCellIter = std::find(mvOwnedCells.at(pComb).begin(), mvOwnedCells.at(pComb).end(), iCellIdx);
                if (pCellIter != mvOwnedCells.at(pComb).end())
                {
                    // Remove the cell.
                    mvOwnedCells.at(pComb).erase(pCellIter);
                    if (!pComb->CombContainsColor(meColor))
                    {
                        // Purge the comb as we have 0 ownership of it.
                        mvOwnedCells.erase(pComb);
                        mvOwnedCombs.erase(pCombIter);
                    }

                    bSuccess = true;
                }
                else
                {
                    // Check to see if the comb even contains the color in question.
                    if (!pComb->CombContainsColor(meColor))
                    {
                        // Purge the comb as we have 0 ownership of it.
                        mvOwnedCombs.erase(pCombIter);
                        bSuccess = true;
                    }
                    else
                    {
                        qCritical("Recurse Err: Tried to remove a cell index we don't own?!");
                    }
                }
            }
            else
            {
                mvOwnedCombs.erase(pCombIter);
                bSuccess = true;
            }
        }
    }

    return bSuccess;
}

CNation& CNation::Merge(CNation& rMother)
{
    // We want to iterate over our owned combs and add them to the new mother.
    for (std::vector<CHoneyComb*>::iterator pCombIter = mvOwnedCombs.end(); pCombIter != mvOwnedCombs.begin(); --pCombIter)
    {
        if (mvOwnedCells.find(*pCombIter) != mvOwnedCells.end())
        {
            // Now we want to iterate over our cell indices and add them to the mother and remove them from us.
            for (std::vector<u32>::iterator pCellIter = mvOwnedCells.at(*pCombIter).end(); pCellIter != mvOwnedCells.at(*pCombIter).begin(); --pCellIter)
            {
                rMother.Add(*pCombIter, *pCellIter);
                Remove(*pCombIter, *pCellIter);
            }

            // Remove the comb!
            Remove(*pCombIter, 0);
        }
    }

    // Done! Return the new mother!
    return rMother;
}

std::vector<CHoneyComb *> CNation::GetAllCombs()
{
    return mvOwnedCombs;
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
