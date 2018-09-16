#include "include/board.h"

// FOR DEBUGGING ONLY!
extern QPointF l_CollisionPoints[NUM_HEX_VERTS];

CBoard::CBoard() : miSize{2}
{
    // Intentionally left blank.
}

CBoard::CBoard(const CBoard& aCls) : miSize{aCls.miSize}
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

    if (miSize > 0)
    {
        // Set the starting position of the first layer.
        float nX = qStartPos.x() + (nTessSz * TESS_X_SHIFT);
        float nY = qStartPos.y() - (nTessSz * TESS_Y_SHIFT);

        qStartPos.setX(nX);
        qStartPos.setY(nY);

        u32 eClr = static_cast<u32>(Cell_Red);

        // Iterate over the layers, adding the appropriate number of combs.
        for (u32 iLyrIdx = 0; iLyrIdx < miSize; ++iLyrIdx)
        {
            // Add the new combs.
            std::vector<SPoint> vLayerPos = CalcTessPos(qStartPos, iLyrIdx, uCellSz, nTessSz);

            qInfo("Positioning %lu honeycombs...", vLayerPos.size());
            for (std::vector<SPoint>::iterator pPtIter = vLayerPos.begin(); pPtIter != vLayerPos.end(); ++pPtIter)
            {
                CHoneyComb* pTmpComb = new CHoneyComb();
                pTmpComb->SetCellSize(uCellSz);
                pTmpComb->SetPosition((*pPtIter));

                // Colorize only the last layer.
                if ((miSize-1) == iLyrIdx)
                {
                    ECellColors eColor = static_cast<ECellColors>(eClr);
                    pTmpComb->SetAllCellColor(eColor);

                    // Create this color's nation.
                    // We use the size of the vector here as it's representative of the comb's index when the comb is pushed on it (0-based index).
                    bool bFoundNation = false;
                    for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
                    {
                        if ((*pNatIter)->GetNationColor() == eColor)
                        {
                            // Add to this nation.
                            (*pNatIter)->Add(pTmpComb, 0);
                            (*pNatIter)->Add(pTmpComb, 1);
                            (*pNatIter)->Add(pTmpComb, 2);
                            (*pNatIter)->Add(pTmpComb, 3);
                            (*pNatIter)->Add(pTmpComb, 4);
                            (*pNatIter)->Add(pTmpComb, 5);
                            (*pNatIter)->Add(pTmpComb, 6);

                            bFoundNation = true;
                            break;
                        }
                    }

                    if (!bFoundNation)
                    {
                        CNation* pTmpNat = new CNation();
                        pTmpNat->Create(eColor, pTmpComb, g_ColorNameMap[eColor]);
                        mvNations.push_back(pTmpNat);
                    }

                    ++eClr;
                    if (static_cast<u32>(Cell_Gray) < eClr) { eClr = static_cast<u32>(Cell_Red); }
                }
                else
                {
                    // Create a white nation.
                    // We use the size of the vector here as it's representative of the comb's index when the comb is pushed on it (0-based index).
                    bool bFoundNation = false;
                    for (std::vector<CNation*>::iterator pNatIter = mvNations.begin(); pNatIter != mvNations.end(); ++pNatIter)
                    {
                        if ((*pNatIter)->GetNationColor() == Cell_White)
                        {
                            // Add to this nation.
                            (*pNatIter)->Add(pTmpComb, 0);
                            (*pNatIter)->Add(pTmpComb, 1);
                            (*pNatIter)->Add(pTmpComb, 2);
                            (*pNatIter)->Add(pTmpComb, 3);
                            (*pNatIter)->Add(pTmpComb, 4);
                            (*pNatIter)->Add(pTmpComb, 5);
                            (*pNatIter)->Add(pTmpComb, 6);

                            bFoundNation = true;
                            break;
                        }
                    }

                    if (!bFoundNation)
                    {
                        CNation* pTmpNat = new CNation();
                        pTmpNat->Create(Cell_White, pTmpComb, g_ColorNameMap[Cell_White]);
                        mvNations.push_back(pTmpNat);
                    }
                }

                mpBoardCombs.push_back(pTmpComb);
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
 * \brief CBoard::GetNeighbors
 *
 * This function will return all the neighbors for the given honeycomb.
 *
 * \note This function will heap-allocate data and it's up to the client to free this memory!
 *
 * \param pComb - Honeycomb to gather the neighbors of.
 * \return CHoneyComb pointer array of the neighbors, or nullptr if no neighbors.
 */
std::vector<CHoneyComb *> CBoard::GetNeighbors(CHoneyComb *pComb)
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
std::vector<CHoneyComb *> CBoard::GetNeighbors(u32 uCombIdx)
{
    return GetNeighbors(GetComb(uCombIdx));
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

/*!
 * \brief CBoard::GetLastCombAttacked
 *
 * This function will return the index of the last honeycomb that a color successfully "attacked". This means that the color gained cells in the comb at the returned index and *may* have filled
 * the comb, taking ownership. The caller will need to determine if this has occurred or not and react accordingly.
 *
 * \param aeClr - The color to search for
 * \return A 0-based index of the comb last attacked, or -1 if the color isn't present.
 */
int CBoard::GetLastCombAttacked(ECellColors aeClr)
{
    return (mColorLastMap.end() != mColorLastMap.find(aeClr)) ? mColorLastMap.at(aeClr) : (-1);
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
    const float c_nCombSize = static_cast<float>(c_uCellRadius * CELL_COMB_RATIO);
    const float c_nCombCircumRadius = (c_nCombSize * TESS_COMBSZ_TO_TESSSZ);
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
