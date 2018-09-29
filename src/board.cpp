#include "include/board.h"

// FOR DEBUGGING ONLY!
QPointF l_CollisionPoints[NUM_HEX_VERTS];

CBoard::CBoard() : miSize{2}, mnCombSz{0}
{
    // Intentionally left blank.
}

CBoard::CBoard(const CBoard& aCls) : miSize{aCls.miSize}, mnCombSz{aCls.mnCombSz}
{
    if (!aCls.mpBoardCombs.empty())
    {
        mpBoardCombs = aCls.mpBoardCombs;
        mColorLastMap = aCls.mColorLastMap;
        mvNations = aCls.mvNations;
    }
}

CBoard::~CBoard()
{
    for (CombIterator pIter = mpBoardCombs.begin(); pIter != mpBoardCombs.end(); ++pIter)
    {
        CHoneyComb *pTmp = (*pIter);
        if (nullptr != pTmp)
        {
            delete pTmp;
        }
    }

    for (std::vector<CNation*>::iterator pNatIter = mvNations.end(); pNatIter != mvNations.begin(); --pNatIter)
    {
        CNation *pTmp = (*pNatIter);
        if (nullptr != pTmp)
        {
            delete pTmp;
        }
    }

    mvNations.clear();
}

CBoard& CBoard::operator =(const CBoard& aCls)
{
    if (this != &aCls)
    {
        miSize = aCls.miSize;
        mpBoardCombs = aCls.mpBoardCombs;
        mColorLastMap = aCls.mColorLastMap;
        mvNations = aCls.mvNations;
    }

    return *this;
}

/*!
 * \brief CBoard::Create
 *
 * This method is used to "create" a new board. This invloves heap-allocating (if needed) new honeycombs and tessellating them accordingly.
 *
 * \param uCellSz - This is the size of the cells for the combs.
 * \param aqCenter - The center of the board.
 */
void CBoard::Create(u32 uCellSz, SPoint aqCenter)
{
    const u32 c_uCellRadius = uCellSz / 2;
    const float c_nCombSize = static_cast<float>(c_uCellRadius * CELL_COMB_RATIO);
    float nTessSz = (c_nCombSize * TESS_COMBSZ_TO_TESSSZ);

    // Explicitly set the center comb.
    SPoint qStartPos = aqCenter;

    CHoneyComb* pTmpComb = new CHoneyComb();
    pTmpComb->SetCellSize(uCellSz);
    pTmpComb->SetPosition(qStartPos);
    mpBoardCombs.push_back(pTmpComb);

    // Add the new cells to the cell map.
    std::vector<CCell*> vCells = pTmpComb->GetCells();
    for (std::vector<CCell*>::iterator iCellIter = vCells.begin(); iCellIter != vCells.end(); ++iCellIter)
    {
        CCell* pCell = (*iCellIter);

        if (nullptr != pCell)
        {
            const SPoint lCellPos = pCell->GetPosition();

            u64 uCellID = static_cast<u32>(lCellPos.mX);
            uCellID = uCellID << (WORD_SZ / 2);
            uCellID += static_cast<u32>(lCellPos.mY);

            // Add the cell to the map.
            mmCellMap.insert(std::pair<u64, CCell*>(uCellID, pCell));

            // Add the cell ID to the correct nation.
            AddCellToNation(Cell_White, uCellID);

            // Set the cell's color.
            pCell->SetColor(Cell_White);
        }
    }

    if (miSize > 0)
    {
        // Set the starting position of the first layer.
        float nX = qStartPos.x() + (nTessSz * TESS_X_SHIFT);
        float nY = qStartPos.y() - (nTessSz * TESS_Y_SHIFT);

        qStartPos.setX(nX);
        qStartPos.setY(nY);

        u32 eClr = static_cast<u32>(Cell_White);

        // Iterate over the layers, adding the appropriate number of combs.
        for (u32 iLyrIdx = 0; iLyrIdx < miSize; ++iLyrIdx)
        {
            // Add the new combs.
            std::vector<SPoint> vLayerPos = CalcTessPos(qStartPos, iLyrIdx, uCellSz, nTessSz);

            qInfo("Positioning %lu honeycombs...", vLayerPos.size());
            for (std::vector<SPoint>::iterator pPtIter = vLayerPos.begin(); pPtIter != vLayerPos.end(); ++pPtIter)
            {
                if ((miSize-1) == iLyrIdx)
                {
                    ++eClr;
                    if (static_cast<u32>(Cell_Gray) < eClr) { eClr = static_cast<u32>(Cell_Red); }
                }

                ECellColors eColor = static_cast<ECellColors>(eClr);

                CHoneyComb* pTmpComb = new CHoneyComb();
                pTmpComb->SetCellSize(uCellSz);
                pTmpComb->SetPosition((*pPtIter));
                mpBoardCombs.push_back(pTmpComb);

                // Add the new cells to the cell map.
                std::vector<CCell*> vCells = pTmpComb->GetCells();
                for (std::vector<CCell*>::iterator iCellIter = vCells.begin(); iCellIter != vCells.end(); ++iCellIter)
                {
                    CCell* pCell = (*iCellIter);

                    if (nullptr != pCell)
                    {
                        const SPoint lCellPos = pCell->GetPosition();

                        u64 uCellID = static_cast<u32>(lCellPos.mX);
                        uCellID = uCellID << (WORD_SZ / 2);
                        uCellID += static_cast<u32>(lCellPos.mY);

                        // Add the cell to the map.
                        mmCellMap.insert(std::pair<u64, CCell*>(uCellID, pCell));

                        // Add the cell ID to the correct nation.
                        AddCellToNation(eColor, uCellID);

                        // Set the cell's color.
                        pCell->SetColor(eColor);
                    }
                }
            }

            // Update the starting position.
            nX = qStartPos.x() + (nTessSz * TESS_X_SHIFT);
            nY = qStartPos.y() - (nTessSz * TESS_Y_SHIFT);

            // Update the tessellation size.
            nTessSz = (nTessSz * TESS_NEAR_TO_FAR);

            qStartPos.setX(nX);
            qStartPos.setY(nY);
        }
    }
}

/*!
 * \brief CBoard::Destroy
 *
 * This method is used to "destroy" an existing board. This involves going through and deleting the individual combs in the comb array, not the array itself!
 */
void CBoard::Destroy()
{
    for (CombIterator pIter = mpBoardCombs.begin(); pIter != mpBoardCombs.end(); ++pIter)
    {
        if (nullptr != *pIter)
        {
            delete (*pIter);
        }
    }

    // Clear the color last map.
    mColorLastMap.clear();
}

/*!
 * \brief CBoard::Draw
 *
 * This function simply iterates over the comb array and calls "Draw" on the combs.
 * \param pPainter - Pointer to a device context to paint to.
 */
void CBoard::Draw(QPainter *pPainter)
{
    if (nullptr != pPainter)
    {
        for (CombIterator pIter = mpBoardCombs.begin(); pIter != mpBoardCombs.end(); ++pIter)
        {
            if (nullptr != (*pIter))
            {
                (*pIter)->Draw(pPainter);
            }
        }

        if (g_cfgVars.mbIsDebug)
        {
            pPainter->setPen(QPen(QBrush(Qt::red), 3));
            pPainter->setBrush(Qt::transparent);
            pPainter->drawPolygon(l_CollisionPoints, NUM_HEX_VERTS);
        }
    }
}

/*!
 * \brief CBoard::GetBoardSize
 *
 * This method returns the number of tessellation rows set for the current/next board.
 *
 * \return unsigned int representing tessellation rows.
 */
u32 CBoard::GetBoardSize()
{
    return miSize;
}

/*!
 * \brief CBoard::GetCombSize
 *
 * This function simply returns the calculated comb size.
 *
 * \return The claculated comb size.
 */
float CBoard::GetCombSize()
{
    return mnCombSz;
}

/*!
 * \brief CBoard::GetCombIterator
 *
 * This function simply returns an iterator object to the board's honeycombs.
 *
 * \return CombIterator pointing to the first honeycomb.
 */
CombIterator CBoard::GetCombIterator()
{
    return mpBoardCombs.begin();
}

/*!
 * \brief CBoard::GetComb
 *
 * This function returns a pointer to the honeycomb referenced by the iterator.
 *
 * \param pIter - Valid honeycomb iterator for the board.
 * \return Honeycomb object being referenced, or nullptr if none exists.
 */
CHoneyComb* CBoard::GetComb(CombIterator pIter)
{
    return (*pIter);
}

/*!
 * \brief CBoard::GetComb [overloaded]
 *
 * This function returns a pointer to the honeycomb at the given index.
 *
 * \param uCombIdx - Index to the comb to return.
 * \return Honeycomb object being referenced, or nullptr if none exists.
 */
CHoneyComb* CBoard::GetComb(u32 uCombIdx)
{
    return (uCombIdx < mpBoardCombs.size()) ? mpBoardCombs[uCombIdx] : nullptr;
}

/*!
 * \brief CBoard::GetNationList
 *
 * This function is used so the CGame class can have a reference to the nation vector for manipulation purposes.
 *
 * \return Pointer to the nation vector.
 */
std::vector<CNation *> CBoard::GetNationList()
{
    return mvNations;
}

std::vector<CCell*> CBoard::GetCellNeighbors(u64 uCellID)
{
    std::vector<CCell*> vNeighbors;

    if (nullptr != mpBoardCombs[0])
    {
        // Begin calculating the collision points.
        // Simple variable to help us track half-height of the polygon.
        const float c_nCircumRadius = mpBoardCombs[0]->GetCombSize() / 2.0f;
        const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

        SPoint lPos;
        lPos.setX((uCellID & 0xffffffff00000000) >> 32);
        lPos.setY(uCellID & 0x00000000ffffffff);

        QString lMsg("Looking for neigbors of (%1, %2)...");
        lMsg = lMsg.arg(lPos.mX).arg(lPos.mY);
        qDebug(lMsg.toStdString().c_str());

        bool bShouldBreak = false;
        if ((801 <= lPos.mX && 803 >= lPos.mX) && (1025 <= lPos.mY && 1027 >= lPos.mY) && g_cfgVars.mbIsDebug)
        {
            bShouldBreak = true;
        }

        float nX = lPos.mX - c_nCircumRadius;
        float nY = lPos.mY;
        float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle); // We start with a negative degree.

        // Begin calculating the points (counter-clockwise, starting at top).
        //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
        std::map<u64, CCell*>::iterator iCellIter;
        for (size_t iIdx = 0; NUM_HEX_VERTS > iIdx; ++iIdx)
        {
            if (g_cfgVars.mbIsDebug) { l_CollisionPoints[iIdx] = QPointF(nX, nY); }

            for (iCellIter = mmCellMap.begin(); iCellIter != mmCellMap.end(); ++iCellIter)
            {
                std::pair<u64, CCell*> lCell = (*iCellIter);
                SPoint lPt(nX, nY);

                if (g_cfgVars.mbIsDebug && nullptr != lCell.second && lCell.second->GetColor() == Cell_White && bShouldBreak)
                {
                    std::raise(SIGINT); // BREAK!
                }

                if (nullptr != lCell.second && lCell.second->PointInHex(lPt))
                {
                    QString lMsg("Found neigbor @ (%1, %2) [Real: %3, %4]");
                    lMsg = lMsg.arg(lPt.mX).arg(lPt.mY);
                    lMsg = lMsg.arg(lCell.second->GetPosition().mX).arg(lCell.second->GetPosition().mY);
                    qDebug(lMsg.toStdString().c_str());

                    vNeighbors.push_back(lCell.second);
                    break;
                }
            }

            // Calculate the next position.
            float nThetaRad = static_cast<float>(nTheta * (M_PI / 180.0f));
            float nXDelta = c_nCircumRadius * cos(nThetaRad);
            float nYDelta = c_nCircumRadius * sin(nThetaRad);

            // Subtract both.
            nX += nXDelta;
            nY += nYDelta;

            nTheta += c_nDegreePerAngle;
            if (nTheta >= MAX_DEGREE)
            {
                nTheta -= MAX_DEGREE;
            }
        }
    }

    return vNeighbors;
}

std::vector<CCell*> CBoard::GetCellNeighbors(CHoneyComb* pComb, u32 uCellIdx)
{
    std::vector<CCell*> lRtn;
    if (nullptr != pComb)
    {
        CCell* pOriginCell = pComb->GetCellAt(uCellIdx);
        if (nullptr != pOriginCell)
        {
            SPoint sPos = pOriginCell->GetPosition();
            u64 uCellID = static_cast<u32>(sPos.mX);
            uCellID = uCellID << (WORD_SZ / 2);
            uCellID += static_cast<u32>(sPos.mY);

            lRtn = GetCellNeighbors(uCellID);
        }
    }

    return lRtn;
}

std::vector<CCell*> CBoard::GetCellNeighbors(u32 uCombIdx, u32 uCellIdx)
{
    return GetCellNeighbors(GetComb(uCombIdx), uCellIdx);
}

/*!
 * \brief CBoard::SetBoardSize
 *
 * This method is used to set the number of tessellation layers the board should use. These layers are the number of "cirlces" that go around the center honeycomb.
 * Default is 2 for a "medium" board. A large board would be "4" and a small board would be "1".
 *
 * \param[in] uSz - Number of layers.
 */
void CBoard::SetBoardSize(u32 uSz)
{
    miSize = uSz;
}

/*!
 * \brief CBoard::CalcTessPos
 *
 * This method is used to calculate all the positions of the honeycombs being tessellated for a given layer. These are calculated using a basic hexagon multiplied by the layer index.
 * Algorithm is as follows:
 *
 *  new SPoint[6 * (iLayerIdx+1)]
 *  for point in array:
 *      if point is hexagon vertex:
 *          calculate next vertex.
 *      calculate X shift amount divided by (layer + 1)
 *      calculate Y shift amount divided by (layer + 1)
 *      shift X by x-shift
 *      shift Y by y-shift
 *      Set point position to (x,y)
 *  done
 *
 * \param aStart - The start position (should be top-most comb).
 * \param iLayerIdx - The layer index we're at.
 * \return Pointer which references a heap-allocated array of SPoint references or nullptr upon error.
 */
std::vector<SPoint> CBoard::CalcTessPos(SPoint& aStart, u32 iLayerIdx, u32 uCellSz, u32 uTessLegLen)
{
    const u32 c_uCellRadius = uCellSz / 2;
    mnCombSz = static_cast<float>(c_uCellRadius * CELL_COMB_RATIO);
    const float c_nCombCircumRadius = (mnCombSz * TESS_COMBSZ_TO_TESSSZ);
    const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f
    const u32 c_iNumPoints = NUM_HEX_VERTS * (iLayerIdx + 1);

    float nX = aStart.x();
    float nY = aStart.y();
    float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle); // We start with a negative degree.

    // Create the array.
    std::vector<SPoint> mpPointArr;
    mpPointArr.push_back(SPoint(nX, nY));

    // Calculate the next position.
    float nThetaRad = static_cast<float>((nTheta - TESS_ROTATION) * (M_PI / 180.0f));
    float nNxtVertX = uTessLegLen * sin(nThetaRad);
    float nNxtVertY = uTessLegLen * cos(nThetaRad);

    // Calculate the "current" position.
    float nXDelta = c_nCombCircumRadius * sin(nThetaRad);
    float nYDelta = c_nCombCircumRadius * cos(nThetaRad);

    const u32 uVarience = static_cast<u32>(uCellSz * 0.05f);

    // Begin calculating the points (counter-clockwise, starting at top).
    //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
    for (size_t iIdx = 1; c_iNumPoints > iIdx; ++iIdx)
    {
        // Set the comb position.
        mpPointArr.push_back(SPoint(nX + nXDelta, nY + nYDelta));

        if (floor(nXDelta) <= floor(nNxtVertX) + uVarience &&
            floor(nXDelta) >= floor(nNxtVertX) - uVarience &&
            floor(nYDelta) <= floor(nNxtVertY) + uVarience &&
            floor(nYDelta) >= floor(nNxtVertY) - uVarience)
        {
            // Add both.
            nX += nXDelta;
            nY += nYDelta;

            // Update theta.
            nTheta += c_nDegreePerAngle;
            if (nTheta >= MAX_DEGREE)
            {
                nTheta -= MAX_DEGREE;
            }

            // Recalculate the next vertex.
            nThetaRad = static_cast<float>((nTheta - TESS_ROTATION) * (M_PI / 180.0f));
            nNxtVertX = uTessLegLen * sin(nThetaRad);
            nNxtVertY = uTessLegLen * cos(nThetaRad);

            // Re-Calculate the next position.
            nXDelta = c_nCombCircumRadius * sin(nThetaRad);
            nYDelta = c_nCombCircumRadius * cos(nThetaRad);
        }
        else
        {
            // Calculate the next position.
            nXDelta += c_nCombCircumRadius * sin(nThetaRad);
            nYDelta += c_nCombCircumRadius * cos(nThetaRad);
        }
    }

    return mpPointArr;
}

void CBoard::AddCellToNation(ECellColors eClr, u64 uCellID)
{
    // Check to see if we have that nation.
    bool bFoundNation = false;
    CNation* pNation = nullptr;
    for (std::vector<CNation*>::iterator iNatIter = mvNations.begin(); iNatIter != mvNations.end(); ++iNatIter)
    {
        pNation = (*iNatIter);
        if (nullptr != pNation && pNation->GetNationColor() == eClr)
        {
            bFoundNation = true;
            break;
        }
    }

    if (nullptr != pNation && bFoundNation)
    {
        pNation->Add(uCellID);
    }
    else
    {
        pNation = new CNation();
        pNation->Create(eClr, g_ColorNameMap[eClr]);
        pNation->Add(uCellID);
        mvNations.push_back(pNation);

        qInfo(QString("Added %1 nation to board!").arg(g_ColorNameMap[eClr]).toStdString().c_str());
    }
}

#if 0
/*!
 * \brief CBoard::GetNeighbors
 *
 * This function will return all the neighbors for the given honeycomb.
 *
 * \note This function will heap-allocate data and it's up to the client to free this memory!
 *
 * \param pComb - Honeycomb to gather the neighbors of.
 * \return CHoneyComb pointer array of the neighbors, or nullptr if no neighbors.
 */
std::vector<CHoneyComb *> CBoard::GetCombNeighbors(CHoneyComb *pComb)
{
    std::vector<CHoneyComb*> vNeighbors; // Maximum of 6 neighbors.
    if (nullptr != pComb)
    {
        // Get this comb's position and size.
        const u32 uCellSz = static_cast<u32>(pComb->GetCellSize() / 2.0f);
        const float c_nCombSize = static_cast<float>(uCellSz * CELL_COMB_RATIO);
        const float c_nCombCircumRadius = (c_nCombSize * TESS_COMBSZ_TO_TESSSZ);
        const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

        SPoint qPos = pComb->GetPosition();

        float nX = qPos.x();
        float nY = qPos.y() - c_nCombCircumRadius;
        float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle);

        // Calculate the tessellation positions that'll be used for collision detection.
        std::vector<SPoint> vCollPos;
        for (size_t iIdx = 0; NUM_HEX_VERTS > iIdx; ++iIdx)
        {
            // Set the vertex position.
            SPoint qPos(nX, nY);
            vCollPos.push_back(qPos);

            // Calculate the next position.
            float nThetaRad = static_cast<float>((nTheta - TESS_ROTATION) * (M_PI / 180.0f));
            float nXDelta = c_nCombCircumRadius * sin(nThetaRad);
            float nYDelta = c_nCombCircumRadius * cos(nThetaRad);

            // Subtract both.
            nX += nXDelta;
            nY += nYDelta;

            nTheta += c_nDegreePerAngle;
            if (nTheta >= MAX_DEGREE)
            {
                nTheta -= MAX_DEGREE;
            }
        }

        // Now, iterate over the combs and collect all 6 neighbors (if possible).
        for (CombIterator pIter = GetCombIterator(); pIter != mpBoardCombs.end(); ++pIter)
        {
            CHoneyComb *pTmpComb = (*pIter);
            if (nullptr != pTmpComb)
            {
                for (std::vector<SPoint>::iterator pCollIter = vCollPos.begin(); pCollIter != vCollPos.end(); ++pCollIter)
                {
                    if (pTmpComb->PointInComb( (*pCollIter) ))
                    {
                        vNeighbors.push_back(pTmpComb);
                        l_CollisionPoints[vNeighbors.size()-1].setX((*pCollIter).x());
                        l_CollisionPoints[vNeighbors.size()-1].setY((*pCollIter).y());
                    }
                }
            }
        }
    }

    return vNeighbors;
}

/*!
 * \brief CBoard::GetNeighbors [overloaded]
 *
 * This function will return all the neighbors for the given honeycomb index.
 *
 * \param uCombIdx - The index of the comb to gather the neighbors of.
 * \return CHoneyComb pointer array of the neighbors, or nullptr if no neighbors.
 */
std::vector<CHoneyComb *> CBoard::GetCombNeighbors(u32 uCombIdx)
{
    return GetCombNeighbors(GetComb(uCombIdx));
}

/*!
 * \brief CBoard::GetCombNeighborsWithColor
 *
 * This function is used to get a honeycomb's neighbors that contain the color we want.
 * \note It's important to note that the search color will be tested against the cells, not the combs themselves.
 *
 * \param pComb - The honeycomb to search from.
 * \param eSearchColor - The color to search for.
 * \return
 */
std::vector<CHoneyComb*> CBoard::GetCombNeighborsWithColor(CHoneyComb *pComb, ECellColors eSearchColor)
{
    std::vector<CHoneyComb*> vNeighbors; // Maximum of 6 neighbors.
    if (nullptr != pComb)
    {
        // Get this comb's position and size.
        const u32 uCellSz = static_cast<u32>(pComb->GetCellSize() / 2.0f);
        const float c_nCombSize = static_cast<float>(uCellSz * CELL_COMB_RATIO);
        const float c_nCombCircumRadius = (c_nCombSize * TESS_COMBSZ_TO_TESSSZ);
        const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

        SPoint qPos = pComb->GetPosition();

        float nX = qPos.x();
        float nY = qPos.y() - c_nCombCircumRadius;
        float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle);

        // Calculate the tessellation positions that'll be used for collision detection.
        std::vector<SPoint> vCollPos;
        for (size_t iIdx = 0; NUM_HEX_VERTS > iIdx; ++iIdx)
        {
            // Set the vertex position.
            SPoint qPos(nX, nY);
            vCollPos.push_back(qPos);

            // Calculate the next position.
            float nThetaRad = static_cast<float>((nTheta - TESS_ROTATION) * (M_PI / 180.0f));
            float nXDelta = c_nCombCircumRadius * sin(nThetaRad);
            float nYDelta = c_nCombCircumRadius * cos(nThetaRad);

            // Subtract both.
            nX += nXDelta;
            nY += nYDelta;

            nTheta += c_nDegreePerAngle;
            if (nTheta >= MAX_DEGREE)
            {
                nTheta -= MAX_DEGREE;
            }
        }

        // Now, iterate over the combs and collect all 6 neighbors (if possible).
        for (CombIterator pIter = GetCombIterator(); pIter != mpBoardCombs.end(); ++pIter)
        {
            CHoneyComb *pTmpComb = (*pIter);
            if (nullptr != pTmpComb)
            {
                for (std::vector<SPoint>::iterator pCollIter = vCollPos.begin(); pCollIter != vCollPos.end(); ++pCollIter)
                {
                    if (pTmpComb->PointInComb( (*pCollIter) ) && pTmpComb->CombContainsColor(eSearchColor))
                    {
                        vNeighbors.push_back(pTmpComb);
                        l_CollisionPoints[vNeighbors.size()-1].setX((*pCollIter).x());
                        l_CollisionPoints[vNeighbors.size()-1].setY((*pCollIter).y());
                    }
                }
            }
        }
    }

    return vNeighbors;
}

/*!
 * \brief CBoard::GetCellNeighborsWithColor
 *
 * This function is used to acquire all the neighbor cells that are the provided color in the provided comb, measuring from the provided cell index.
 *
 * \param pComb - The comb to search from.
 * \param uCellIdx - The cell to acquire the neighbors of.
 * \param eSearchColor - The color to search for.
 * \return Vector of pairs, each pair contains a CCell pointer and it's associated honeycomb (as a pointer).
 */
std::vector<std::pair<CHoneyComb*, u32>> CBoard::GetCellNeighborsWithColor(CHoneyComb *pComb, u32 uCellIdx, ECellColors eSearchColor)
{
    std::vector<std::pair<CHoneyComb*, u32>> lRtn;
    if (nullptr != pComb)
    {
        std::vector<std::pair<CHoneyComb*, u32>> lCellNeighbors = GetCellNeighbors(pComb, uCellIdx);
        if (0 < lCellNeighbors.size())
        {
            std::vector<std::pair<CHoneyComb*, u32>>::iterator lNeighborIter = lCellNeighbors.begin();
            for (lNeighborIter = lCellNeighbors.begin(); lNeighborIter != lCellNeighbors.end(); ++lNeighborIter)
            {
                std::pair<CHoneyComb*, u32> lNeigbor = (*lNeighborIter);
                CCell* pNeighbor = lNeigbor.first->GetCellAt(lNeigbor.second);
                if (nullptr != pNeighbor)
                {
                    if (pNeighbor->GetColor() == eSearchColor)
                    {
                        lRtn.push_back(lNeigbor);
                    }
                }
            }
        }
    }

    return lRtn;
}

/*!
 * \brief CBoard::GetCellNeighbors
 *
 * This function will return all the neighbor cells for the given cell in a given comb.
 *
 * \param pComb - The comb to get neighbors for.
 * \param uCellIdx - The index of the cell in the comb.
 * \return Vector of pairs, each pair contains a CCell pointer and it's associated honeycomb (as a pointer).
 */
std::vector<std::pair<CHoneyComb*, u32>> CBoard::GetCellNeighbors(CHoneyComb* pComb, u32 uCellIdx)
{
    std::vector<std::pair<CHoneyComb*, u32>> lRtn;
    CCell* pOriginCell = pComb->GetCellAt(uCellIdx);
    if (nullptr != pComb && nullptr != pOriginCell)
    {
        if (0 == uCellIdx)
        {
            // Return all of the other cells.
            for (u32 uIdx = 1; NUM_HEX_VERTS >= uIdx; ++uIdx)
            {
                lRtn.push_back(std::pair<CHoneyComb*, u32>(pComb, uIdx));
            }
        }
        else
        {
            // Grab the comb's neighbors.
            std::vector<CHoneyComb*> vNeighbors = GetCombNeighbors(pComb);
            vNeighbors.push_back(pComb);

            // Begin calculating the collision points.
            // Simple variable to help us track half-height of the polygon.
            const float c_nCombSize = pComb->GetCombSize();
            const float c_nCircumRadius = c_nCombSize / 2.0f;
            const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

            float nX = pOriginCell->GetPosition().mX - c_nCircumRadius;
            float nY = pOriginCell->GetPosition().mY;
            float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle); // We start with a negative degree.

            // Begin calculating the points (counter-clockwise, starting at top).
            //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
            for (size_t iIdx = 1; (NUM_HEX_VERTS+1) > iIdx; ++iIdx)
            {
                // See if we have a collision with any combs (ourself included).
                SPoint collPoint(nX, nY);
                for (std::vector<CHoneyComb*>::iterator pNeighborIter = vNeighbors.begin(); pNeighborIter != vNeighbors.end(); ++pNeighborIter)
                {
                    CHoneyComb* pTmpComb = (*pNeighborIter);
                    u32 uCellIdx = pTmpComb->GetCellIdxAtPoint(collPoint);
                    if (uCellIdx < NUM_HEX_VERTS)
                    {
                        lRtn.push_back(std::pair<CHoneyComb*, u32>(pTmpComb, uCellIdx));
                    }
                }

                // Calculate the next position.
                float nThetaRad = static_cast<float>(nTheta * (M_PI / 180.0f));
                float nXDelta = c_nCircumRadius * cos(nThetaRad);
                float nYDelta = c_nCircumRadius * sin(nThetaRad);

                // Subtract both.
                nX += nXDelta;
                nY += nYDelta;

                nTheta += c_nDegreePerAngle;
                if (nTheta >= MAX_DEGREE)
                {
                    nTheta -= MAX_DEGREE;
                }
            }
        }
    }

    return lRtn;
}

/*!
 * \brief CBoard::GetCellNeighbors [overloaded]
 *
 * This function will return all the neighbor cells for the given cell in a given comb.
 *
 * \param uCombIdx - The index of the comb.
 * \param uCellIdx - The index of the cell in the comb.
 * \return Vector of pairs, each pair contains a CCell pointer and it's associated honeycomb (as a pointer).
 */
std::vector<std::pair<CHoneyComb*, u32>> CBoard::GetCellNeighbors(u32 uCombIdx, u32 uCellIdx)
{
    return GetCellNeighbors(GetComb(uCombIdx), uCellIdx);
}
#endif //#if 0
