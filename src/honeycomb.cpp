#include "include/honeycomb.h"

// ================================ Begin CCell Implementation ================================ //
CCell::CCell() : mbIsValid{false}, mnSize{0.0f}, mPosition{SPoint(0.0f,0.0f)}, meClr{Cell_White}
{
    // Intentionally left blank.
}

CCell::CCell(const CCell& aCls) : mbIsValid{aCls.mbIsValid}, mnSize{aCls.mnSize}, mPosition{aCls.mPosition}, meClr{aCls.meClr}
{
    // Intentionally left blank.
}

CCell::~CCell()
{
    mnSize = 0.0f;
    mPosition = SPoint(0.0f,0.0f);
    meClr = Cell_White;
}

CCell& CCell::operator =(const CCell& aCls)
{
    if (this != &aCls)
    {
        mbIsValid = aCls.mbIsValid;
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
        if (mbIsValid)
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
            // Instantiate a new set of verticies.
            QPointF pPts[NUM_HEX_VERTS];
            memset(pPts, 0, sizeof(SPoint) * NUM_HEX_VERTS);

            const float c_nCircumRadius = mnSize / 2.0f;
            const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

            float nX = mPosition.x();
            float nY = mPosition.y() - c_nCircumRadius;
            float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle); // We start with a negative degree.

            // Begin calculating the points (counter-clockwise, starting at top).
            //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
            for (size_t iIdx = 0; NUM_HEX_VERTS > iIdx; ++iIdx)
            {
                // Set the vertex position.
                pPts[iIdx].setX(nX);
                pPts[iIdx].setY(nY);

                // Calculate the next position.
                float nThetaRad = static_cast<float>(nTheta * (M_PI / 180.0f));
                float nXDelta = c_nCircumRadius * sin(nThetaRad);
                float nYDelta = c_nCircumRadius * cos(nThetaRad);

                // Subtract both.
                nX += nXDelta;
                nY += nYDelta;

                nTheta += c_nDegreePerAngle;
                if (nTheta >= MAX_DEGREE)
                {
                    nTheta -= MAX_DEGREE;
                }
            }

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
                case Cell_Brown: pPainter->setBrush(QBrush(QColor(125, 50, 0))); break;
                case Cell_Gray: pPainter->setBrush(QBrush(QColor(125, 125, 125))); break;
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

bool CCell::PointInHex(SPoint &aPt)
{
    const float c_nInscribedRadius = (static_cast<float>(sqrt(3)) / 2.0) * (mnSize / 2.0f);
    float nOppSqr = pow( abs(aPt.x() - mPosition.x()), 2.0);
    float nAdjSqr = pow( abs(aPt.y() - mPosition.y()), 2.0);
    float nDelta = sqrt(nOppSqr + nAdjSqr);

    return (nDelta < c_nInscribedRadius) ? true : false;
}

float CCell::GetSize()
{
    return mnSize;
}

const SPoint& CCell::GetCenter()
{
    return mPosition;
}

const SPoint& CCell::GetPosition()
{
    return mPosition;
}

ECellColors CCell::GetColor()
{
    return meClr;
}

bool CCell::IsValid()
{
    return mbIsValid;
}

void CCell::SetSize(const float anSize)
{
    mnSize = anSize;
    if (0.0f < mnSize) { mbIsValid = true; }
}

void CCell::SetCenter(const SPoint &aqCenter)
{
    mPosition = aqCenter;
    if (0.0f < mnSize) { mbIsValid = true; }
}

void CCell::SetPosition(const SPoint &aqPosition)
{
    mPosition = aqPosition;
    if (0.0f < mnSize) { mbIsValid = true; }
}

void CCell::SetColor(ECellColors aeClr)
{
    meClr = aeClr;
}
// ================================ End CCell Implementation ================================ //

// ================================ Begin CHoneycomb Implementation ================================ //
CHoneyComb::CHoneyComb() : mnCellSize{0.0f}, mPosition{SPoint(0.0f, 0.0f)}, meCombColor{Cell_White}
{
    // Intentionally left blank.
}

CHoneyComb::CHoneyComb(const CHoneyComb& aCls) : mnCellSize{aCls.mnCellSize}, mPosition{aCls.mPosition}, meCombColor{aCls.meCombColor}, mpCells{aCls.mpCells}
{
    // Intentionally left blank.
}

CHoneyComb::~CHoneyComb()
{
    mnCellSize = 0.0f;
    mPosition = SPoint(0.0f, 0.0f);
}

CHoneyComb& CHoneyComb::operator =(const CHoneyComb& aCls)
{
    if (this != &aCls)
    {
        mnCellSize = aCls.mnCellSize;
        mPosition = aCls.mPosition;
        meCombColor = aCls.meCombColor;
        mpCells = aCls.mpCells;
    }

    return *this;
}

CCell *CHoneyComb::operator [](size_t iIdx)
{
    if (c_iMaxCells > iIdx)
    {
        return mpCells[iIdx];
    }

    return mpCells[c_iMaxCells-1];
}

std::vector<CCell*> CHoneyComb::operator *()
{
    return mpCells;
}

bool CHoneyComb::Draw(QPainter *pPainter)
{
    bool bSuccess = true;
    if (nullptr != pPainter && IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end() && bSuccess; ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->IsValid())
            {
                bSuccess = pTmpCell->Draw(pPainter);
            }
        }
    }

    return bSuccess;
}

bool CHoneyComb::PointInComb(SPoint &aPt)
{
    bool bSuccess = false;

    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end() && !bSuccess; ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->IsValid())
            {
                bSuccess = pTmpCell->PointInHex(aPt);
            }
        }
    }

    return bSuccess;
}

bool CHoneyComb::CombIsAllColor(ECellColors aeClr)
{
    bool bSuccess = true;

    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end() && bSuccess; ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->IsValid())
            {
                if (pTmpCell->GetColor() != aeClr)
                {
                    bSuccess = false;
                    break;
                }
            }
        }
    }

    return bSuccess;
}

bool CHoneyComb::CombContainsColor(ECellColors aeClr)
{
    bool bSuccess = false;

    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end() && !bSuccess; ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->IsValid())
            {
                if (pTmpCell->GetColor() == aeClr)
                {
                    bSuccess = true;
                    break;
                }
            }
        }
    }

    return bSuccess;
}

bool CHoneyComb::IsInitialized()
{
    if (0 < mnCellSize)
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
    return static_cast<float>(mnCellSize * CELL_COMB_RATIO);
}

const SPoint& CHoneyComb::GetPosition()
{
    return mPosition;
}

std::vector<CCell*> CHoneyComb::GetCells()
{
    return mpCells;
}

CCell* CHoneyComb::GetCellAt(u32 iCellIdx)
{
    return (mpCells.size() > iCellIdx) ? mpCells[iCellIdx] : mpCells[mpCells.size()-1];
}

CCell* CHoneyComb::GetCellNotColor(ECellColors aeClr)
{
    CCell* rCell = nullptr;

    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end(); ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->GetColor() != aeClr)
            {
                rCell = pTmpCell;
                break;
            }
        }
    }

    return rCell;
}

CCell* CHoneyComb::GetCellIsColor(ECellColors aeClr)
{
    CCell* rCell = nullptr;

    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end(); ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->GetColor() == aeClr)
            {
                rCell = pTmpCell;
                break;
            }
        }
    }

    return rCell;
}

size_t CHoneyComb::GetCellIdxNotColor(ECellColors eIsNotColor)
{
    size_t iCellIdx = 0;
    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end(); ++pIter, ++iCellIdx)
        {
            CCell* pTmp = (*pIter);
            if (pTmp->GetColor() != eIsNotColor)
            {
                break;
            }
        }
    }

    return iCellIdx;
}

size_t CHoneyComb::GetCellIdxColor(ECellColors eIsColor)
{
    size_t iCellIdx = 0;
    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end(); ++pIter, ++iCellIdx)
        {
            CCell* pTmp = (*pIter);
            if (pTmp->GetColor() == eIsColor)
            {
                break;
            }
        }
    }

    return iCellIdx;
}

ECellColors CHoneyComb::GetCombColor()
{
    return meCombColor;
}

void CHoneyComb::SetCellSize(const float anSize)
{
    mnCellSize = anSize;

    if (IsInitialized()) { RecalcPositions(); }
}

void CHoneyComb::SetPosition(const SPoint& aqPosition)
{
    mPosition = aqPosition;

    if (IsInitialized()) { RecalcPositions(); }
}

void CHoneyComb::SetCombColor(ECellColors aeClr)
{
    if (IsInitialized())
    {
        meCombColor = aeClr;
    }
}

void CHoneyComb::SetAllCellColor(ECellColors aeClr)
{
    if (IsInitialized())
    {
        for (std::vector<CCell*>::iterator pIter = mpCells.begin(); pIter != mpCells.end(); ++pIter)
        {
            CCell *pTmpCell = (*pIter);
            if (pTmpCell->IsValid())
            {
                pTmpCell->SetColor(aeClr);
            }
        }
        meCombColor = aeClr;
    }
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

        // FIRST: Clear the cell vector to prevent weirdness.
        while (mpCells.begin() != mpCells.end())
        {
            mpCells.pop_back();
        }

        // Simple variable to help us track half-height of the polygon.
        const float c_nCombSize = GetCombSize();
        const float c_nCircumRadius = c_nCombSize / 2.0f;
        const float c_nDegreePerAngle = 180.0f / 3.0f; // Should be 60.0f

        float nX = mPosition.x();
        float nY = mPosition.y();
        float nTheta = static_cast<float>(MAX_DEGREE - c_nDegreePerAngle); // We start with a negative degree.

        // Explicitly set the center comb.
        CCell *pTmpCell = new CCell();
        pTmpCell->SetSize(mnCellSize);
        pTmpCell->SetPosition(SPoint(nX, nY));
        mpCells.push_back(pTmpCell);

        nX -= c_nCircumRadius;

        // Begin calculating the points (counter-clockwise, starting at top).
        //!\NOTE: Our hexagons have the long-leg vertical, meaning they're pointed at the top. (height > width)
        for (size_t iIdx = 1; (NUM_HEX_VERTS+1) > iIdx; ++iIdx)
        {
            // Set the cell position.
            CCell *pTmpCell = new CCell();
            pTmpCell->SetSize(mnCellSize);
            pTmpCell->SetPosition(SPoint(nX, nY));
            mpCells.push_back(pTmpCell);

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

        // DONE!
    }
}

// ================================ End CHoneycomb Implementation ================================ //
