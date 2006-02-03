#include "moveorderdata.h"

using namespace sakusen;

OrderData* MoveOrderData::newCopy(void) const
{
  return new MoveOrderData(*this);
}

