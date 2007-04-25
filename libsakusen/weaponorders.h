#ifndef WEAPONORDERS_H
#define WEAPONORDERS_H

#include "order.h"
#include "weapontargettype.h"
#include "ref.h"

namespace sakusen {

class LIBSAKUSEN_API WeaponOrders {
  public:
    WeaponOrders();
  private:
    WeaponOrders(
        WeaponTargetType targetType,
        const Position& targetPoint,
        const Orientation& targetOrientation,
        const Ref<ICompleteUnit>& targetUnit,
        const Ref<ISensorReturns>& targetSensorReturns
      );
    
    WeaponTargetType targetType;
    Position targetPosition;
    Orientation targetOrientation;
    Ref<ICompleteUnit> targetUnit;
    Ref<ISensorReturns> targetSensorReturns;
  public:
    /** \name accessors */
    /*@{*/
    WeaponTargetType getTargetType() const { return targetType; }
    bool isTargetValid() const;
    Point<sint32> getTargetPosition() const;
    Orientation getTargetOrientation() const { return targetOrientation; }
    Ref<ICompleteUnit> getTargetUnit() const { return targetUnit; }
    Point<sint16> getTargetVelocity() const;
    /*@}*/

    void clear();
    
    /** \brief Update according to the instructions in the given order */
    void update(const Order& order);

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static WeaponOrders load(IArchive&, const loadArgument*);
};

}

#endif // WEAPONORDERS_H

