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

void CNation::Create(ECellColors eClr, QString sName)
{
    meColor = eClr;
    msName = sName;
}

void CNation::Destroy()
{
    // Simply purge the lists, do NOT delete anything as we do NOT own it!
    mvOwnedCells.clear();
}

bool CNation::Add(u64 uCellID)
{
    bool bSuccess = false;

    if (mvOwnedCells.end() == std::find(mvOwnedCells.begin(), mvOwnedCells.end(), uCellID))
    {
        mvOwnedCells.push_back(uCellID);
    }
    else
    {
        std::string sMsg = QString("ERR: We already own the cell with ID %1!").arg(uCellID).toStdString();
        qCritical(sMsg.c_str());
    }

    return bSuccess;
}

bool CNation::Remove(u64 uCellID)
{
    bool bSuccess = false;

    std::vector<u64>::iterator iCellIter = std::find(mvOwnedCells.begin(), mvOwnedCells.end(), uCellID);
    if (mvOwnedCells.end() != iCellIter)
    {
        mvOwnedCells.erase(iCellIter);
    }
    else
    {
        std::string sMsg = QString("ERR: We don't own the cell with ID %1!").arg(uCellID).toStdString();
        qCritical(sMsg.c_str());
    }

    return bSuccess;
}

// !! NOT USED FOR NOW !!
CNation* CNation::Merge(CNation* pMother)
{
    // We don't own them anymore!
    mvOwnedCells.clear();

    // Done! Return the new mother!
    return pMother;
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

std::vector<u64> CNation::GetCellIDs()
{
    return mvOwnedCells;
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
