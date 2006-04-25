#ifndef UNITORDERS_H
#define UNITORDERS_H

#include "libsakusen-global.h"

#include "order.h"
#include "ordercondition.h"
#include "targettype.h"
#include "orientation.h"

namespace sakusen {

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
        const AngularVelocity& tAV
      );
  public:
    UnitOrders();
  private:
    /* conditioned orders */
    Order orders[orderCondition_max];
    /* The last order given - the one which awaits its success or failure */
    Order currentOrder;
    /* current goals which have been set by orders */
    LinearTargetType linearTarget; /* Whether we're currently trying to
                                      achieve a position or a velocity */
    Point<sint32> targetPosition; /* The position we're trying to reach */
    Point<sint16> targetVelocity; /* The velocity we're trying to achieve */
    RotationalTargetType rotationalTarget;
    Orientation targetOrientation; /* The orientation we're trying to reach */
    AngularVelocity targetAngularVelocity; /* The angular velocity we're trying
					      to achieve */
  public:
    inline const Order& getOrder(OrderCondition c) const {
      assert(c>=0);
      assert(c<orderCondition_max);
      return orders[c];
    }
    inline void enqueueOrder(OrderCondition c, const Order& order) {
      assert(c>=0);
      assert(c<orderCondition_max);
      orders[c] = order;
    }
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

    void acceptOrder(OrderCondition condition);
    void clearQueue();
    inline void clearCurrent() { currentOrder = Order(); }

    void store(OArchive&) const;
    static UnitOrders load(IArchive&);
};

}

#endif // UNITORDERS_H

