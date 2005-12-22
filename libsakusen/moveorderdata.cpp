#include "moveorderdata.h"

OrderData* MoveOrderData::newCopy(void) const
{
  return new MoveOrderData(*this);
}

