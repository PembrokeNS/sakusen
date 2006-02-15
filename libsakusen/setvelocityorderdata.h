#ifndef SETVELOCITYORDERDATA_H
#define SETVELOCITYORDERDATA_H

#include "orderdata.h"
#include "point.h"
#include "iarchive.h"

namespace sakusen {

class LIBSAKUSEN_API SetVelocityOrderData : public OrderData {
  private:
    SetVelocityOrderData();
  public:
    SetVelocityOrderData(const Point<sint16>& t) : OrderData(), target(t) {}
    SetVelocityOrderData(IArchive& in) : OrderData() { in >> target; }
    ~SetVelocityOrderData() {};
  private:
    Point<sint16> target;
  public:
    Point<sint32> getTarget(void) const { return target; }
    OrderData* newCopy(void) const;
    void store(OArchive&) const;
};

}

#endif // SETVELOCITYORDERDATA_H

