#ifndef GAME_H
#define GAME_H

#include <QImage>
#include "include/nation.h"
#include "include/board.h"

/*!
 * \brief The CDice class
 *
 * This is a simple class used to provide an arbitrary-sided die. This can range anywhere from D-3 to D-MAX_INT. The class will seed the C++ STL Pseudo-RNG once (during construction) and
 * then continue to use the seeded RNG to generate "random" rolls. These rolls are "one-shot", meaning they are NOT re-rolled to prevent duplicates. This class does track the last produced roll,
 * however. That value though, is not used by anything other than potential tracking.
 */
class CDice
{
public:
    CDice();
    CDice(const CDice& aCls);
    ~CDice();

    CDice& operator=(const CDice& aCls);

    u32 Roll(u32 uMin = 3, u32 uMax = 0xffffffff);

    u32 GetLastRoll();

private:
    u32 muLastRoll;
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
    void NewGame(u32 iDiceMax = 0xffffffff, u32 uCellSz = 128, SPoint qCenter = SPoint(1024, 1024));
    void Play(ECellColors eAggressor, ECellColors eVictim);
    void EndGame();
    void Destroy();

    std::pair<bool, QString> MoveColor(ECellColors eAggressor, ECellColors eVictim, u32 uMvAmnt = 3);

    u32 DummyRoll();

    void Draw();

    // Getters.
    u32 GetDiceMax();
    CBoard* GetBoard();

    bool NationExists(ECellColors eColor);

    QImage* GetCanvas();

    // Setters.
    void SetDiceMax(u32 iMaxium = 0xffffffff);

private:
    u32 DoFloodFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt);
    u32 DoInfectionFill(CNation* aAggrNation, CNation* aVictimNation, u32 uMvAmnt);

    CDice *mpDice; //!< Pointer to the dice used to make decisions.
    CBoard *mpBoard; //!< Pointer to the active game board.
    QImage *mpCanvas; //!< The drawing canvas for the game.
    std::vector<CNation*> mvNations; //!< Vector of pointers to the current (live) nations at play.
    u32 muDiceMax; //!< The maximum roll amount for a dice "throw".
    std::string msTmpFileName; //!< Temporary filename for the image to write to.
};

#endif // GAME_H
