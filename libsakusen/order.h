#ifndef ORDER_H
#define ORDER_H

#include "ordertype.h"
#include "orderdata.h"
#include "oarchive.h"
#include "iarchive.h"

namespace sakusen {

class LIBSAKUSEN_API Order {
  public:
    Order();
    Order(const Order& copy);
    Order(const OrderData& d);
    ~Order();
    Order& operator=(const Order& copy);
  private:
    /** What is this an order to do? */
    OrderType type;
    const OrderData* data; /**< Pointer to abstract class OrderData, which has
                              subclasses for the diffferent types of data
                              necessary */
  public:
    /** \name acccessors */
    /*@{*/
    inline bool isRealOrder(void) const {
      return getType() != orderType_none;
    }
    inline OrderType getType(void) const { return type; }
    inline const MoveOrderData& getMoveData(void) const {
      return *dynamic_cast<const MoveOrderData*>(data);
    }
    inline const SetVelocityOrderData& getSetVelocityData(void) const {
      return *dynamic_cast<const SetVelocityOrderData*>(data);
    }
    inline const TargetSensorReturnsOrderData& getTargetSensorReturnsData(void) 
      const {
      return *dynamic_cast<const TargetSensorReturnsOrderData*>(data);
    }
    /*@}*/

    void store(OArchive&) const;
    static Order load(IArchive&);
};

}

#endif // ORDER_H

