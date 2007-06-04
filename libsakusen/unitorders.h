#ifndef UNITORDERS_H
#define UNITORDERS_H

#include "libsakusen-global.h"

#include "order.h"
#include "ordercondition.h"
#include "targettype.h"
#include "orientation.h"
#include "weaponorders.h"

namespace sakusen {

/** \brief Holds the orders for a Unit.
 *
 * A unit's current order, and its queued (conditional) orders, are stored in
 * this structure.
 */
class LIBSAKUSEN_API UnitOrders {
  private:
    UnitOrders(
        /* conditioned orders */
        Order ord[orderCondition_max],
        const Order& cO,
        /* current goals which have been set by orders */
        const LinearTargetType& lT,
        const Point<sint32>& tP,
        const Point<sint16>& tV,
        const RotationalTargetType& rT,
        const Orientation& tO,
        const AngularVelocity& tAV,
        const std::vector<WeaponOrders>& weaponOrders
      );
  public:
    UnitOrders(uint16 numWeapons);
  private:
    /** One order per condition. The order may be the nop order. */
    Order orders[orderCondition_max];
    /** The last order given - the one which awaits its success or
     * failure */
    Order currentOrder;
    /** \name Current goals which have been set by orders */
    //@{
    /** Whether we're currently trying to achieve a position or a velocity */
    LinearTargetType linearTarget;
    /** The position we're trying to reach */
    Point<sint32> targetPosition;
    /** The velocity we're trying to achieve */
    Point<sint16> targetVelocity; 
    /** Whether we're currently trying to achieve an orientation or an angular
     * velocity. */
    RotationalTargetType rotationalTarget;
    /** The orientation we're trying to reach */
    Orientation targetOrientation; 
    /** The angular velocity we're trying to achieve */
    AngularVelocity targetAngularVelocity; 
    //@}
    /** The order status of the unit's weapons */
    std::vector<WeaponOrders> weaponOrders;
  public:
    /** \name accessors */
    /*@{*/
    /** Gets the order with order condition \p c */
    inline const Order& getOrder(OrderCondition c) const {
      assert(c>=0);
      assert(c<orderCondition_max);
      return orders[c];
    }
    /** Gets the order currently being followed. */
    inline const Order& getCurrentOrder(void) const { return currentOrder; }
    inline LinearTargetType getLinearTarget(void) const { return linearTarget; }
    inline const Point<sint32>& getTargetPosition(void) const {
      return targetPosition;
    }
    inline const Point<sint16>& getTargetVelocity(void) const {
      return targetVelocity;
    }
    inline RotationalTargetType getRotationalTarget(void) const {
      return rotationalTarget;
    }
    inline const Orientation& getTargetOrientation(void) const {
      return targetOrientation;
    }
    inline const AngularVelocity& getTargetAngularVelocity(void) const {
      return targetAngularVelocity;
    }
    inline const std::vector<WeaponOrders>& getWeaponsOrders(void) const {
      return weaponOrders;
    }
    inline std::vector<WeaponOrders>& getWeaponsOrders(void) {
      return weaponOrders;
    }
    /*@}*/

    /** Queues the order \p order, on condition \p c */
    inline void enqueueOrder(OrderCondition c, const Order& order) {
      assert(c>=0);
      assert(c<orderCondition_max);
      orders[c] = order;
    }

    void acceptOrder(OrderCondition condition);
    void clearQueue();

    /** \brief Reset the current order to the nop order */
    inline void clearCurrent() { currentOrder = Order(); }

    typedef PlayerID loadArgument;
    void store(OArchive&) const;
    static UnitOrders load(IArchive&, const loadArgument*);
};

}

#endif // UNITORDERS_H

