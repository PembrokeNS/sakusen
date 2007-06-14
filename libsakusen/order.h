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
    #define GET_DATA(type) \
    inline const type##OrderData& get##type##Data(void) const { \
      return dynamic_cast<const type##OrderData&>(*data); \
    }
    GET_DATA(Move)
    GET_DATA(SetVelocity)
    GET_DATA(TargetWeapon)
    GET_DATA(TargetPosition)
    GET_DATA(TargetPositionOrientation)
    GET_DATA(TargetUnit)
    GET_DATA(TargetSensorReturns)
    #undef GET_DATA
    /*@}*/

    void store(OArchive&) const;
    static Order load(IArchive&, const DeserializationContext&);
};

}

#endif // ORDER_H

