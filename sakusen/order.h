#ifndef LIBSAKUSEN__ORDER_H
#define LIBSAKUSEN__ORDER_H

#include <sakusen/ordertype.h>
#include <sakusen/orderdata.h>
#include <sakusen/oarchive.h>
#include <sakusen/iarchive.h>

namespace sakusen {

class LIBSAKUSEN_API Order {
  public:
    Order();
    Order(const OrderData* data);
  private:
    /** What is this an order to do? */
    OrderType type;
    /** Pointer to abstract class OrderData, which has subclasses for the
     * diffferent types of data which can occur */
    OrderData::ConstPtr data; 
  public:
    /** \name acccessors */
    /*@{*/
    inline bool isRealOrder(void) const {
      return getType() != orderType_none;
    }
    inline OrderType getType(void) const { return type; }

    /* To remind that update here is needed by causing a compile error when a
     * new enum value is added. */
    SAKUSEN_STATIC_ASSERT(orderType_max == 11);
    
    #define SAKUSEN_ORDER_GET_DATA(type) \
    inline const type##OrderData& get##type##Data(void) const { \
      return dynamic_cast<const type##OrderData&>(*data); \
    }
    SAKUSEN_ORDER_GET_DATA(SetVelocity)
    SAKUSEN_ORDER_GET_DATA(Move)
    SAKUSEN_ORDER_GET_DATA(SetAngularVelocity)
    SAKUSEN_ORDER_GET_DATA(Orient)
    SAKUSEN_ORDER_GET_DATA(TargetNone)
    SAKUSEN_ORDER_GET_DATA(TargetNumber)
    SAKUSEN_ORDER_GET_DATA(TargetWeapon)
    SAKUSEN_ORDER_GET_DATA(TargetPosition)
    SAKUSEN_ORDER_GET_DATA(TargetFrame)
    SAKUSEN_ORDER_GET_DATA(TargetUnit)
    SAKUSEN_ORDER_GET_DATA(TargetSensorReturns)
    #undef SAKUSEN_ORDER_GET_DATA
    /*@}*/

    void store(OArchive&) const;
    static Order load(IArchive&, const DeserializationContext&);
};

}

#endif // LIBSAKUSEN__ORDER_H

