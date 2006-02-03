#ifndef SETVELOCITYORDERDATA_H
#define SETVELOCITYORDERDATA_H

#include "orderdata.h"
#include "point.h"

namespace sakusen {

class SetVelocityOrderData : public OrderData {
  private:
    SetVelocityOrderData();
  public:
    SetVelocityOrderData(const Point<sint16>& t) : OrderData(), target(t) {}
    ~SetVelocityOrderData() {};
  private:
    Point<sint16> target;
  public:
    Point<sint32> getTarget(void) const { return target; }
    OrderData* newCopy(void) const;
};

}

#endif // SETVELOCITYORDERDATA_H

