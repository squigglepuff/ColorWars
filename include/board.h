#ifndef BOARD_H
#define BOARD_H

#include "include/nation.h"

typedef std::vector<CHoneyComb*>::iterator CombIterator; //!< This is used as a helper type for ease of iterating over the board combs.

/*!
 * \brief The CBoard class
 *
 * This class is designed to tessellate the honeycombs into a playing board for the game to use. This should be done only once during the "Create" method and when the board is finished,
 * it should be destroyed using the "Destroy" method. This class should also provide a seamless way of getting neighbor combs for a given honeycomb. This will enable quick searching and
 * minimize tick time.
 */
class CBoard
{
public:
    CBoard();
    CBoard(const CBoard& aCls);
    ~CBoard();

    // Overloads.
    CBoard& operator=(const CBoard& aCls);

    // Workers.
    void Create(u32 uCellSz, QPointF aqCenter);
    void Destroy();

    void Draw(QPainter *pPainter = nullptr);

    // Getters.
    u32 GetBoardSize();

    CombIterator GetCombIterator();

    CHoneyComb* GetComb(CombIterator pIter);
    CHoneyComb* GetComb(u32 uCombIdx = 0);

    CHoneyComb **GetNeighbors(CHoneyComb *pComb = nullptr);
    CHoneyComb **GetNeighbors(u32 uCombIdx = 0);

    std::vector<CNation*> GetNationList();

    int GetLastCombAttacked(ECellColors aeClr);

    // Setters.
    void SetBoardSize(u32 uSz = 2);

private:
    std::vector<QPointF> CalcTessPos(QPointF& aStart, u32 iLayerIdx, u32 uCellSz, u32 uTessLegLen);

    u32 miSize; //!< Number of tessellation layers for the board. (Default = 2)
    std::vector<CHoneyComb*> mpBoardCombs; //!< Board honeycombs. (array of pointers)
    std::map<ECellColors, u32> mColorLastMap; //!< Map used as reference for finding the last comb a color successfully "attacked".
    std::vector<CNation*> mvNations; //!< Vector of pointers to the current (live) nations at play.
};

#endif // BOARD_H
