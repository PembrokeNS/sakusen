#ifndef MOVEORDERDATA_H
#define MOVEORDERDATA_H

#include "point.h"
#include "orderdata.h"

class LIBSAKUSEN_API MoveOrderData : public OrderData {
  private:
    MoveOrderData();
  public:
    MoveOrderData(const Point<sint32>& t) : OrderData(), target(t) {}
    ~MoveOrderData() {};
  private:
    Point<sint32> target;
  public:
    const Point<sint32>& getTarget(void) const { return target; }
    OrderData* newCopy(void) const;
};

#endif // MOVEORDERDATA_H

