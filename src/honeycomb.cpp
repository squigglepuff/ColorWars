#include "include/honeycomb.h"

// ================================ Begin CCell Implementation ================================ //
CCell::CCell() : mnSize{0.0f}, mPosition{QPointF(0.0f,0.0f)}, meClr{Cell_White}
{
    // Intentionally left blank.
}

CCell::CCell(const CCell& aCls) : mnSize{aCls.mnSize}, mPosition{aCls.mPosition}, meClr{aCls.meClr}
{
    // Intentionally left blank.
}

CCell::~CCell()
{
    mnSize = 0.0f;
    mPosition = QPointF(0.0f,0.0f);
    meClr = Cell_White;
}

CCell& CCell::operator =(const CCell& aCls)
{
    if (this != &aCls)
    {
        mnSize = aCls.mnSize;
        mPosition = aCls.mPosition;
        meClr = aCls.meClr;
    }

    return *this;
}

bool CCell::Draw(QPainter *pPainter)
{
    bool bSuccess = false;
    if (nullptr != pPainter)
    {
        if (!mPosition.isNull() && mnSize > 0.0f)
        {
            /*
             * These values are used in calculations. They're approximates, so accuracy isn't going to happen.
             * These are based off of "docs/hexagon_dissection.png".
             * These Are defined as
                    * "long-leg" : "short-leg" (1:2)
                    * "long-leg" : "short-start" (1:4)
                    * "half-width" = 0.86 : "short-leg" (0.86:1)
             * Static_cast is more than likely unecessary, we're just doing it to ensure we have floats.
             */
            // Simple variable to help us track half-height of the polygon.
            const float c_nHalfHeight = mnSize / 2.0f;

            // Instantiate a new set of verticies.
            QPointF pPts[NUM_HEX_VERTS];
            memset(pPts, 0, sizeof(QPointF) * NUM_HEX_VERTS);

            // Begin calculating the points (clockwise, starting at top).
            //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
            float nX = mPosition.x();
            float nY = mPosition.y() - c_nHalfHeight;
            pPts[0].setX(nX);
            pPts[0].setY(nY);

            // Top-right vertex.
            nX += (mnSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY += (mnSize * HEX_SHORT_START);
            pPts[1].setX(nX);
            pPts[1].setY(nY);

            // Bottom-right vertex.
            nY += (mnSize * HEX_LONG_SHORT);
            pPts[2].setX(nX);
            pPts[2].setY(nY);

            // Bottom vertex.
            nX -= (mnSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY += (mnSize * HEX_SHORT_START);
            pPts[3].setX(nX);
            pPts[3].setY(nY);

            // Bottom-left vertex.
            nX -= (mnSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
            nY -= (mnSize * HEX_SHORT_START);
            pPts[4].setX(nX);
            pPts[4].setY(nY);

            // Top-left vertex.
            nY -= (mnSize * HEX_LONG_SHORT);
            pPts[5].setX(nX);
            pPts[5].setY(nY);

            // Done, now we want to set the painter to draw the hexagon correctly.
            pPainter->setPen(QPen(QBrush(Qt::black), 2.0f));

            // Set the fill color.
            switch (meClr)
            {
                case Cell_White: pPainter->setBrush(QBrush(QColor(255, 255, 255))); break;
                case Cell_Red: pPainter->setBrush(QBrush(QColor(255, 0, 0))); break;
                case Cell_Orange: pPainter->setBrush(QBrush(QColor(255, 175, 0))); break;
                case Cell_Yellow: pPainter->setBrush(QBrush(QColor(255, 255, 0))); break;
                case Cell_Lime: pPainter->setBrush(QBrush(QColor(175, 255, 0))); break;
                case Cell_Green: pPainter->setBrush(QBrush(QColor(0, 175, 0))); break;
                case Cell_Cyan: pPainter->setBrush(QBrush(QColor(0, 255, 255))); break;
                case Cell_Blue: pPainter->setBrush(QBrush(QColor(0, 0, 255))); break;
                case Cell_Purple: pPainter->setBrush(QBrush(QColor(125, 0, 255))); break;
                case Cell_Magenta: pPainter->setBrush(QBrush(QColor(255, 0, 255))); break;
                case Cell_Pink: pPainter->setBrush(QBrush(QColor(255, 0, 125))); break;
                default: pPainter->setBrush(QBrush(QColor(255, 255, 255))); break;
            }

            // Draw the points!
            pPainter->drawPolygon(pPts, NUM_HEX_VERTS);

            // Set success!
            bSuccess = true;
        }
        else
        {
            qCritical("ASSERT ERROR: Hexagon geometry is invalid!");
        }
    }
    else
    {
        qCritical("ASSERT ERROR: pPainter is NULL! Please try again!");
    }

    return bSuccess;
}

bool CCell::PointInHex(const QPoint& aPt)
{
    bool bSuccess = false;
    if (!aPt.isNull())
    {
        const float c_nInscribedRadius = (static_cast<float>(sqrt(3)) / 2.0) * (mnSize / 2.0f);
        float nOppSqr = pow( abs(aPt.x() - mPosition.x()), 2.0);
        float nAdjSqr = pow( abs(aPt.y() - mPosition.y()), 2.0);
        float nDelta = sqrt(nOppSqr + nAdjSqr);

        bSuccess = (nDelta < c_nInscribedRadius) ? true : false;
    }

    return bSuccess;
}

float CCell::GetSize()
{
    return mnSize;
}

const QPointF& CCell::GetCenter()
{
    return mPosition;
}

const QPointF& CCell::GetPosition()
{
    return mPosition;
}

ECellColors CCell::GetColor()
{
    return meClr;
}

void CCell::SetSize(const float anSize)
{
    mnSize = anSize;
}

void CCell::SetCenter(const QPointF &aqCenter)
{
    if (!aqCenter.isNull())
    {
        mPosition = aqCenter;
    }
}

void CCell::SetPosition(const QPointF &aqPosition)
{
    if (!aqPosition.isNull())
    {
        mPosition = aqPosition;
    }
}

void CCell::SetColor(ECellColors aeClr)
{
    meClr = aeClr;
}
// ================================ End CCell Implementation ================================ //

// ================================ Begin CHoneycomb Implementation ================================ //
CHoneyComb::CHoneyComb() : mnCellSize{0.0f}, mPosition{QPointF(0.0f, 0.0f)}, mpCells{nullptr}
{
    mpCells = new CCell[c_iMaxCells];
}

CHoneyComb::CHoneyComb(const CHoneyComb& aCls) : mnCellSize{aCls.mnCellSize}, mPosition{aCls.mPosition}, mpCells{nullptr}
{
    mpCells = new CCell[c_iMaxCells];
    memcpy(mpCells, aCls.mpCells, (sizeof(CCell) * (c_iMaxCells)));
}

CHoneyComb::~CHoneyComb()
{
    mnCellSize = 0.0f;
    mPosition = QPointF(0.0f, 0.0f);

    if (nullptr != mpCells)
    {
        delete[] mpCells;
    }
}

CHoneyComb& CHoneyComb::operator =(const CHoneyComb& aCls)
{
    if (this != &aCls)
    {
        mnCellSize = aCls.mnCellSize;
        mPosition = aCls.mPosition;

        // Sanity: Make sure we have a valid array of cells, if not allocate some.
        if (nullptr == mpCells)
        {
            mpCells = new CCell[c_iMaxCells];
        }

        memcpy(mpCells, aCls.mpCells, (sizeof(CCell) * (c_iMaxCells)));
    }

    return *this;
}

CCell& CHoneyComb::operator [](size_t iIdx)
{
    if (c_iMaxCells > iIdx)
    {
        return mpCells[iIdx];
    }

    return mpCells[c_iMaxCells-1];
}

CCell* CHoneyComb::operator *()
{
    return mpCells;
}

bool CHoneyComb::Draw(QPainter *pPainter)
{
    bool bSuccess = true;
    if (nullptr != pPainter && IsInitialized())
    {
        for (CCell* pIter = mpCells; nullptr != pIter && bSuccess; ++pIter)
        {
            bSuccess = pIter->Draw(pPainter);
        }
    }

    return bSuccess;
}

bool CHoneyComb::PointInComb(const QPoint &aPt)
{
    bool bSuccess = false;

    if (IsInitialized())
    {
        for (CCell* pIter = mpCells; nullptr != pIter && !bSuccess; ++pIter)
        {
            bSuccess = pIter->PointInHex(aPt);
        }
    }

    return bSuccess;
}

bool CHoneyComb::CombIsAllColor(ECellColors aeClr)
{
    bool bSuccess = true;

    if (IsInitialized())
    {
        for (CCell* pIter = mpCells; nullptr != pIter && bSuccess; ++pIter)
        {
            if ((*pIter).GetColor() != aeClr)
            {
                bSuccess = false;
                break;
            }
        }
    }

    return bSuccess;
}

bool CHoneyComb::IsInitialized()
{
    if (nullptr != mpCells && 0 < mnCellSize)
    {
        return true;
    }

    return false;
}

float CHoneyComb::GetCellSize()
{
    return mnCellSize;
}

float CHoneyComb::GetCombSize()
{
    return static_cast<float>(mnCellSize * c_nCellCombRatio);
}

const QPointF& CHoneyComb::GetPosition()
{
    return mPosition;
}

CCell* CHoneyComb::GetCells()
{
    return mpCells;
}

CCell* CHoneyComb::GetCellNotColor(ECellColors aeClr)
{
    CCell* pCell = nullptr;

    if (IsInitialized())
    {
        for (CCell* pIter = mpCells; nullptr != pIter; ++pIter)
        {
            if ((*pIter).GetColor() != aeClr)
            {
                pCell = pIter;
                break;
            }
        }
    }

    return pCell;
}

void CHoneyComb::SetCellSize(const float anSize)
{
    mnCellSize = anSize;

    // (Re)Calculate the positons of the cells.
    RecalcPositions();
}

void CHoneyComb::SetPosition(const QPointF& aqPosition)
{
    mPosition = aqPosition;

    // (Re)Calculate the positons of the cells.
    RecalcPositions();
}

void CHoneyComb::RecalcPositions()
{
    if (IsInitialized())
    {
        /*
         * Begin recalculation.
         *
         * These positions are actually calculated using the same hexagon calculations for the cells. The only difference being that the
         * hexagon here is rotated 90-degrees clockwise (so instead of moving up first, we move right).
         *
         * This means that this method makes use of the consts in friend class CCell.
         */

        // Simple variable to help us track half-height of the polygon.
        const float c_nCombSize = GetCombSize();
        const float c_nHalfWidth = c_nCombSize / 2.0f;

        // Begin calculating the points (clockwise, starting at top).
        //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
        float nX = mPosition.x();
        float nY = mPosition.y();

        // Center cell [0].
        mpCells[0].SetSize(mnCellSize);
        mpCells[0].SetPosition(QPointF(nX, nY));

        // Right Cell [1].
        nX = nX + c_nHalfWidth;
        mpCells[1].SetSize(mnCellSize);
        mpCells[1].SetPosition(QPointF(nX, nY));

        // Bottom-Right Cell [2].
        nX = nX - (c_nHalfWidth / 2.0f);
        nY = nY + (c_nCombSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
        mpCells[2].SetSize(mnCellSize);
        mpCells[2].SetPosition(QPointF(nX, nY));

        // Bottom-left cell [3].
        nX = nX - c_nHalfWidth;
        mpCells[3].SetSize(mnCellSize);
        mpCells[3].SetPosition(QPointF(nX, nY));

        // Left cell [4]
        nX = nX - (c_nHalfWidth / 2.0f);
        nY = nY - (c_nCombSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
        mpCells[4].SetSize(mnCellSize);
        mpCells[4].SetPosition(QPointF(nX, nY));

        // Top-left cell [5].
        nX = nX + (c_nHalfWidth / 2.0f);
        nY = nY - (c_nCombSize * HEX_LONG_SHORT) * HEX_HALF_WIDTH;
        mpCells[5].SetSize(mnCellSize);
        mpCells[5].SetPosition(QPointF(nX, nY));

        // Top-right cell [6].
        nX = nX + c_nHalfWidth;
        mpCells[6].SetSize(mnCellSize);
        mpCells[6].SetPosition(QPointF(nX, nY));

        // DONE!
    }
}

// ================================ End CHoneycomb Implementation ================================ //
