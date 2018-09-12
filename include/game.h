#ifndef GAME_H
#define GAME_H

#include "include/board.h"

/*!
 * \brief The CDice class
 *
 * This is a simple class used to provide an arbitrary-sided die. This can range anywhere from D-3 to D-MAX_INT. The class will seed the C++ STL Pseudo-RNG once (during construction) and
 * then continue to use the seeded RNG to generate "random" rolls. These rolls are "one-shot", meaning they are NOT re-rolled to prevent duplicates.
 */
class CDice
{
public:
    CDice();
    CDice(const CDice&);
    ~CDice();

    CDice& operator=(const CDice&);

    u32 Roll(u32 uMin = 3, u32 uMax = 0xffffffff);
};

/*!
 * \brief The CGame class
 *
 * This class is used to control the game board along with the logic of the game itself. This class also provides an instantiation of CDice which is used to perform a "roll".
 * The roll determines how a color moves, if allowed. The rolls are setup like so:
 *  [x - y](25% of range){center of range}  --->  Move 3 spaces
 *  [x - y](10% of range){center of range}  --->  Move 6 spaces
 *  [x - y](5% of range){center of range}   --->  Move 9 spaces
 *  [x - y](1% of range){center of range}   --->  Overtake
 *
 * The color movement algorithm is a "fill" algorithm, the logic will attempt to fill the current honeycomb and then expand into another hexagon.
 */
class CGame
{
public:
    CGame();
    CGame(const CGame& aCls);
    ~CGame();

    CGame& operator=(const CGame& aCls);

    // Workers.
    void NewGame(u32 iDiceMax = 0xffffffff);
    void EndGame();

    std::pair<bool, QString> MoveColor(ECellColors eAggressor, ECellColors eVictim);

    u32 DummyRoll();

    // Getters.
    u32 GetDiceMax();

    // Setters.
    void SetDiceMax(u32 iMaxium = 0xffffffff);

    // Signals.
signals:
    void needsRedraw();

private:
    CDice *mpDice;
    CBoard *mpBoard;
};

#endif // GAME_H
