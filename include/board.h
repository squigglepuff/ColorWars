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
    void Create(u32 uCellSz, SPoint aqCenter);
    void Destroy();

    void Draw(QPainter *pPainter = nullptr);

    // Getters.
    u32 GetBoardSize();
    float GetCombSize();

    CombIterator GetCombIterator();

    CHoneyComb* GetComb(CombIterator pIter);
    CHoneyComb* GetComb(u32 uCombIdx = 0);

#if 0
    std::vector<CHoneyComb*> GetCombNeighbors(CHoneyComb *pComb = nullptr);
    std::vector<CHoneyComb*> GetCombNeighbors(u32 uCombIdx = 0);

    std::vector<CHoneyComb*> GetCombNeighborsWithColor(CHoneyComb *pComb = nullptr, ECellColors eSearchColor = Cell_White);
    std::vector<std::pair<CHoneyComb *, u32> > GetCellNeighborsWithColor(CHoneyComb *pComb = nullptr, u32 uCellIdx = 0, ECellColors eSearchColor = Cell_White);

#endif //#if 0
    std::vector<CCell*> GetCellNeighbors(u64 uCellID);
    std::vector<CCell*> GetCellNeighbors(CHoneyComb* pComb = nullptr, u32 uCellIdx = 0);
    std::vector<CCell*> GetCellNeighbors(u32 uCombIdx = 0, u32 uCellIdx = 0);

    std::vector<CNation*> GetNationList();

    // Setters.
    void SetBoardSize(u32 uSz = 2);

private:
    std::vector<SPoint> CalcTessPos(SPoint& aStart, u32 iLayerIdx, u32 uCellSz, u32 uTessLegLen);
    void AddCellToNation(ECellColors eClr, u64 uCellID);

    u32 miSize; //!< Number of tessellation layers for the board. (Default = 2)
    float mnCombSz; //!< The size of a single honeycomb object (used in positioning).
    std::vector<CHoneyComb*> mpBoardCombs; //!< Board honeycombs. (array of pointers)
    std::map<ECellColors, u32> mColorLastMap; //!< Map used as reference for finding the last comb a color successfully "attacked".
    std::vector<CNation*> mvNations; //!< Vector of pointers to the current (live) nations at play.
    std::map<u64, CCell*> mmCellMap; //!< This is a cell map for easy cell location based on X,Y coordinates.
};

#endif // BOARD_H
