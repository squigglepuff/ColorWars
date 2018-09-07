#ifndef BOARD_H
#define BOARD_H

#include "include/honeycomb.h"

typedef CHoneyComb** CombIterator; //!< This is used as a helper type for ease of iterating over the board combs.

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
    void Create(u32 uCellSz);
    void Destroy();

    void Draw(QPainter *pPainter = nullptr);

    // Getters.
    u32 GetBoardSize();

    CombIterator GetCombIterator();

    CHoneyComb* GetComb(CombIterator pIter = nullptr);
    CHoneyComb* GetComb(u32 uCombIdx = 0);

    CombIterator GetNeighbors(CHoneyComb *pComb = nullptr);
    CombIterator GetNeighbors(u32 uCombIdx = 0);

    // Setters.
    void SetBoardSize(u32 uSz = 2);

private:
    CHoneyComb **mpBoardCombs; //!< Board honeycombs. (array of pointers)
    u32 miSize; //!< Number of tessellation layers for the board. (Default = 2)
};

#endif // BOARD_H
