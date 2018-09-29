#ifndef NATION_H
#define NATION_H

#include "include/honeycomb.h"

/*!
 * \brief The CNation class
 *
 * This class is used to represent a color nation. This simply means all the honeycombs that contain a certain color are tracked by this class. This allows the game logic to be simpler as it
 * only need be aware of the nations, who "know" where their borders are and who they border to some extent. This means that when a "move" is made, it'll be a color nation attacking
 * another color nation instead of just raw colors attacking each other. This allows the nations to always be aware of where the updated borders lie.
 *
 * \note This class takes NO ownership of the cells/combs it tracks! Those are left in position of their respective combs/board(s).
 */
class CNation
{
public:
    CNation();
    CNation(const CNation& aCls);
    ~CNation();

    CNation& operator=(const CNation& aCls);

    CNation *operator <<(CNation &aChild);
    CNation *operator >>(CNation &aParent);

    // Workers.
    void Create(ECellColors eClr = Cell_White, QString sName = "White");
    void Destroy();

    bool Add(u64 uCellID);
    bool Remove(u64 uCellID);

    CNation* Merge(CNation* pMother);

    // Getters.
    std::vector<u64> GetCellIDs();

    u32 GetNationSize();
    ECellColors GetNationColor();
    QString GetNationName();

    // Setters.
    void SetNationColor(ECellColors eColor);
    void SetNationName(QString sName);

private:
    std::vector<u64> mvOwnedCells; //!< Map of owned cells of certain combs.
    ECellColors meColor; //!< Color of this nation.
    QString msName; //!< Name of this nation.
};

#endif // NATION_H
