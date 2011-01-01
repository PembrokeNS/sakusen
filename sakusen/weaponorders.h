#ifndef LIBSAKUSEN__WEAPONORDERS_H
#define LIBSAKUSEN__WEAPONORDERS_H

#include <sakusen/order.h>
#include <sakusen/weapontargettype.h>
#include <sakusen/ref.h>

namespace sakusen {

/** \brief Encapsulates orders for a particular weapon.
 *
 * Each weapon can be individually targeted in a handful of ways; this class
 * keeps track of the way and the specific target data.  e.g., if it is
 * targeted at a position, then the position is stored.
 *
 * \todo We could use a boost::variant to collapse the private members and save
 * memory (possible because each member after targetType is valid only for a
 * particular value of targetType). */
class LIBSAKUSEN_API WeaponOrders {
  public:
    WeaponOrders();
  private:
    WeaponOrders(
        WeaponTargetType targetType,
        const Position& targetPoint,
        const Frame& targetFrame,
        const Ref<ICompleteUnit>& targetUnit,
        const Ref<ISensorReturns>& targetSensorReturns,
        const NumberTarget targetNumber
      );

    WeaponTargetType targetType;
    Position targetPosition;
    Frame targetFrame;
    Ref<ICompleteUnit> targetUnit;
    Ref<ISensorReturns> targetSensorReturns;
    NumberTarget targetNumber;
  public:
    /** \name accessors */
    /*@{*/
    WeaponTargetType getTargetType() const { return targetType; }
    bool isTargetValid() const;
    Point<sint32> getTargetPosition() const;
    const Frame& getTargetFrame() const;
    Ref<ICompleteUnit> getTargetUnit() const { return targetUnit; }
    Point<sint16> getTargetVelocity() const;
    NumberTarget getTargetNumber() const;
    /*@}*/

    void clear();

    /** \brief Update according to the instructions in the given order */
    void update(const Order& order);

    void store(OArchive&) const;
    static WeaponOrders load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__WEAPONORDERS_H

