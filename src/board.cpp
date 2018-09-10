#include "include/board.h"

CBoard::CBoard() : mpBoardCombs{nullptr}, miSize{2}
{
    // Intentionally left blank.
}

CBoard::CBoard(const CBoard& aCls) : mpBoardCombs{nullptr}, miSize{aCls.miSize}
{
    if (nullptr != aCls.mpBoardCombs)
    {
        const size_t c_uNumItems = SZ(aCls.mpBoardCombs);

        mpBoardCombs = new CHoneyComb*[c_uNumItems];
        for (size_t uIdx = 0; uIdx < c_uNumItems; ++uIdx)
        {
            if (nullptr != aCls.mpBoardCombs[uIdx])
            {
                mpBoardCombs[uIdx] = new CHoneyComb();
                memcpy(mpBoardCombs[uIdx], aCls.mpBoardCombs[uIdx], sizeof(CHoneyComb));
            }
        }
    }
}

CBoard::~CBoard()
{
    if (nullptr != mpBoardCombs)
    {
        for (CombIterator pIter = mpBoardCombs; nullptr != pIter; ++pIter)
        {
            if (nullptr != *pIter)
            {
                delete (*pIter);
            }
        }

        delete[] mpBoardCombs;
    }
}

CBoard& CBoard::operator =(const CBoard& aCls)
{
    if (this != &aCls)
    {
        miSize = aCls.miSize;

        const size_t c_uNumItems = SZ(aCls.mpBoardCombs);

        mpBoardCombs = new CHoneyComb*[c_uNumItems];
        for (size_t uIdx = 0; uIdx < c_uNumItems; ++uIdx)
        {
            if (nullptr != aCls.mpBoardCombs[uIdx])
            {
                mpBoardCombs[uIdx] = new CHoneyComb();
                memcpy(mpBoardCombs[uIdx], aCls.mpBoardCombs[uIdx], sizeof(CHoneyComb));
            }
        }
    }

    return *this;
}

/*!
 * \brief CBoard::Create
 *
 * This method is used to "create" a new board. This invloves heap-allocating (if needed) new honeycombs and tessellating them accordingly.
 *
 * \param uCellSz - This is the size of the cells for the combs.
 */
void CBoard::Create(u32 uCellSz, QPointF aqCenter)
{
    if (nullptr != mpBoardCombs)
    {
        // Get this comb's position and size.
        const float nCombSize = static_cast<float>(uCellSz * CELL_COMB_RATIO);
        float nTessSz = (nCombSize * TESS_COMBSZ_TO_TESSSZ);

        // Set the center comb.
        float nX = static_cast<float>(aqCenter.x());
        float nY = static_cast<float>(aqCenter.y());
        u32 uCombIdx = 0;

        if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
        mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
        mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
        ++uCombIdx;

        // Now, iterate over the combs and collect all 6 neighbors (if possible).
        for (u32 uLayerIdx = 0; miSize > uLayerIdx; ++uLayerIdx)
        {
            // Setup our position variables.
            // Top position-vertex.
            nX = nX + (nTessSz * TESS_X_SHIFT);
            nY = nY - (nTessSz * TESS_Y_SHIFT);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Top-right position-vertex.
            nX += (nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY += (nTessSz * HEX_SHORT_START);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Bottom-right position-vertex.
            nY += (nTessSz * HEX_LONG_SHORT);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Bottom position-vertex.
            nX -= (nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY += (nTessSz * HEX_SHORT_START);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Bottom-left position-vertex.
            nX -= (nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY -= (nTessSz * HEX_SHORT_START);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Top-left position-vertex.
            nY -= (nTessSz * HEX_LONG_SHORT);

            if (nullptr == mpBoardCombs[uCombIdx]) { mpBoardCombs[uCombIdx] = new CHoneyComb(); }
            mpBoardCombs[uCombIdx]->SetCellSize(uCellSz);
            mpBoardCombs[uCombIdx]->SetPosition(QPointF(nX, nY));
            ++uCombIdx;

            // Update the tessellation size.
            nTessSz = (nTessSz * TESS_NEAR_TO_FAR);
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
    if (nullptr != mpBoardCombs)
    {
        for (CombIterator pIter = mpBoardCombs; nullptr != pIter; ++pIter)
        {
            if (nullptr != *pIter)
            {
                delete (*pIter);
            }
        }
    }
}

/*!
 * \brief CBoard::Draw
 *
 * This function simply iterates over the comb array and calls "Draw" on the combs.
 * \param pPainter - Pointer to a device context to paint to.
 */
void CBoard::Draw(QPainter *pPainter)
{
    if (nullptr != pPainter && nullptr != mpBoardCombs)
    {
        // code
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
    return mpBoardCombs;
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
    return (nullptr != pIter) ? (*pIter) : nullptr;
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
    return (uCombIdx < SZ(mpBoardCombs)) ? mpBoardCombs[uCombIdx] : nullptr;
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
CombIterator CBoard::GetNeighbors(CHoneyComb *pComb)
{
    CHoneyComb** pvNeighbors = nullptr; // Maximum of 6 neighbors.
    if (nullptr != pComb && nullptr != mpBoardCombs)
    {
        pvNeighbors = new CHoneyComb*[NUM_HEX_VERTS]; // Maximum of 6 neighbors.
        if (nullptr != pvNeighbors)
        {
            // Get this comb's position and size.
            const float c_nTessSz = (pComb->GetCombSize() * TESS_COMBSZ_TO_TESSSZ);
            const QPointF& c_qPos = pComb->GetPosition();

            // Now, iterate over the combs and collect all 6 neighbors (if possible).
            u32 iCombIdx = 0;
            for (CombIterator pIter = GetCombIterator(); nullptr != pIter; ++pIter)
            {
                // Setup our collision variables.
                // Top collision-vertex.
                float nX = c_qPos.x() + (c_nTessSz * TESS_X_SHIFT);
                float nY = c_qPos.y() - (c_nTessSz * TESS_Y_SHIFT);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }

                // Top-right collision-vertex.
                nX += (c_nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
                nY += (c_nTessSz * HEX_SHORT_START);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }

                // Bottom-right collision-vertex.
                nY += (c_nTessSz * HEX_LONG_SHORT);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }

                // Bottom collision-vertex.
                nX -= (c_nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
                nY += (c_nTessSz * HEX_SHORT_START);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }

                // Bottom-left collision-vertex.
                nX -= (c_nTessSz * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
                nY -= (c_nTessSz * HEX_SHORT_START);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }

                // Top-left collision-vertex.
                nY -= (c_nTessSz * HEX_LONG_SHORT);
                if ((*pIter)->PointInComb(QPoint(nX, nY))) { pvNeighbors[iCombIdx] = (*pIter); ++iCombIdx; continue; }
            }
        }
    }

    return pvNeighbors;
}

/*!
 * \brief CBoard::GetNeighbors [overloaded]
 *
 * This function will return all the neighbors for the given honeycomb index.
 *
 * \param uCombIdx - The index of the comb to gather the neighbors of.
 * \return CHoneyComb pointer array of the neighbors, or nullptr if no neighbors.
 */
CombIterator CBoard::GetNeighbors(u32 uCombIdx)
{
    return GetNeighbors(GetComb(uCombIdx));
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
 *  new QPointF[6 * (iLayerIdx+1)]
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
 * \note This function returns some heap-allocated data that is does NOT take ownership of! It's up to the caller to free this memory!
 *
 * \param aStart - The start position (should be top-most comb).
 * \param iLayerIdx - The layer index we're at.
 * \return Pointer which references a heap-allocated array of QPointF references or nullptr upon error.
 */
QPointF* CBoard::CalcTessPos(QPointF& aStart, u32 iLayerIdx)
{
    QPointF *mpPointArr = nullptr;

    return mpPointArr;
}
