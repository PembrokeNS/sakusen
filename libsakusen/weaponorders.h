#ifndef WEAPONORDERS_H
#define WEAPONORDERS_H

#include "order.h"
#include "weapontargettype.h"
#include "ref.h"

namespace sakusen {

class WeaponOrders {
  public:
    WeaponOrders();
  private:
    WeaponOrders(
        WeaponTargetType targetType,
        const Ref<ISensorReturns>& targetSensorReturns
      );
    
    WeaponTargetType targetType;
    Ref<ISensorReturns> targetSensorReturns;
  public:
    /** \brief Update according to the instructions in the given order */
    void update(const Order& order);

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static WeaponOrders load(IArchive&, const loadArgument*);
};

}

#endif // WEAPONORDERS_H

