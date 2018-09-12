#ifndef _HONEYCOMB_H
#define _HONEYCOMB_H

#include <QPainter>
#include "include/globals.h"

#if !defined(_CUSTOM_TYPES_SET)
typedef unsigned char byte;
typedef byte u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define _CUSTOM_TYPES_SET
#endif // #if !defined(_CUSTOM_TYPES_SET)

#if !defined(NUM_HEX_VERTS)
#define NUM_HEX_VERTS (6)
#endif // #if !defined(NUM_HEX_VERTS)

#if !defined(SZ)
#define SZ(x) (sizeof(x)/sizeof(*(x)))
#endif // #if !defined(SZ)

#if !defined(_HEX_RATIOS)
#define HEX_LONG_SHORT  (static_cast<float>(1.0f / 2.0f))
#define HEX_SHORT_START  (static_cast<float>(1.0f / 4.0f))
#define HEX_HALF_WIDTH  (static_cast<float>(0.864f / 1.0f))
#define _HEX_RATIOS
#endif // #if !defined(_HEX_RATIOS)

#if !defined(_CELL_COLORS)
#define _CELL_COLORS
enum ECellColors
{
    Cell_White,
    Cell_Red,
    Cell_Orange,
    Cell_Yellow,
    Cell_Lime,
    Cell_Green,
    Cell_Cyan,
    Cell_Blue,
    Cell_Purple,
    Cell_Magenta,
    Cell_Pink
};
#endif // #if !defined(_CELL_COLORS)


/*!
 * \brief The CCell class
 *
 * This class is used to represent and operate on honeycomb cells. These cells are simply hexagons. Each one is described using approximate values pulled from
 * "docs/hexagon_dissection.png". These values are NOT intented to be accurate, only "good enough".
 * This class is also responsible for drawing the object to a device context (usually QPainter or HDC). This is important as this means that the object MUST take a device context
 * as an argument for it's "Draw" method.
 *
 * \note All the calculations in this class are approximate!
 */
class CCell
{
public:
    CCell();
    CCell(const CCell& aCls);
    ~CCell();

    // Overloads.
    CCell& operator =(const CCell& aCls);

    // Workers.
    bool Draw(QPainter *pPainter = nullptr);

    bool PointInHex(const QPoint& aPt);

    // Getters.
    float GetSize();
    const QPointF& GetCenter();
    const QPointF& GetPosition();
    ECellColors GetColor();

    bool IsValid();

    // Setters.
    void SetSize(const float anSize);
    void SetCenter(const QPointF& aqCenter);
    void SetPosition(const QPointF& aqPosition);
    void SetColor(ECellColors aeClr = Cell_White);

private:
    bool mbIsValid; //!< Is this cell valid (has position and size)?
    float mnSize; //!< The size of the hexagon.
    QPointF mPosition; //!< The position of the hexagon in pixels.
    ECellColors meClr; //!< Color to fill the cell with.
};


/*!
 * \brief The CHoneyComb class
 *
 * This class is used to represent and manage honeycomb objects. These objects are comprised of CCell objects, which represent cells of the honeycomb. The honeycomb and it's cells are described
 * using approximate values pulled from "docs/honeycomb_dissection.png". These values are NOT intended to be accurate, only "good enough".
 *
 * It's important to note that this class doesn't actually draw anything, but has a "Draw" method. This method simply iterates over the cells and cascades into their "Draw" methods.
 *
 * \note All the calculations in this class are approximate!
 */
class CHoneyComb
{
    // Constants!
    const u32 c_iMaxCells = (NUM_HEX_VERTS+1); //!< Maxium number of cells for a honeycomb. We add 1 as honeycombs have a center cell.

public:
    CHoneyComb();
    CHoneyComb(const CHoneyComb& aCls);
    ~CHoneyComb();

    // Overloads.
    CHoneyComb& operator =(const CHoneyComb& aCls);
    CCell& operator [](size_t iIdx); // Used to index through the cells.
    std::vector<CCell> operator *(); // Used to return all cells.

    // Workers.
    bool Draw(QPainter *pPainter = nullptr);

    bool PointInComb(const QPoint& aPt);

    bool CombIsAllColor(ECellColors aeClr);
    bool CombContainsColor(ECellColors aeClr);

    // Getters.
    bool IsInitialized();

    float GetCellSize();
    float GetCombSize();

    ECellColors GetCombColor();

    const QPointF& GetPosition();

    std::vector<CCell> GetCells();
    CCell &GetCellNotColor(ECellColors aeClr);
    CCell &GetCellIsColor(ECellColors aeClr);

    size_t GetCellIdxNotColor(ECellColors eIsNotColor);
    size_t GetCellIdxColor(ECellColors eIsColor);

    // Setters.
    void SetCellSize(const float anSize);
    void SetPosition(const QPointF& aqPosition);
    void SetCombColor(ECellColors aeClr = Cell_White);

private:
    void RecalcPositions();

    float mnCellSize; //!< The size of the cells in pixels.
    QPointF mPosition; //!< The position of the honeycomb (and the center cell).
    ECellColors meCombColor; //!< Color of the entire honycomb.

    std::vector<CCell> mpCells; //!< Heap-allocated cells for the honeycomb.
};

#endif // #ifndef _HONEYCOMB_H
