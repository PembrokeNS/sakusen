#ifndef WEAPONORDERS_H
#define WEAPONORDERS_H

#include "order.h"

namespace sakusen {

class WeaponOrders {
  public:
    /** \brief Update according to the instructions in the given order */
    void update(const Order& order);

    void store(OArchive&) const;
    static WeaponOrders load(IArchive&);
};

}

#endif // WEAPONORDERS_H

