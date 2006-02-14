#include "moveorderdata.h"

using namespace sakusen;

OrderData* MoveOrderData::newCopy(void) const
{
  return new MoveOrderData(*this);
}

void MoveOrderData::store(OArchive& out) const
{
  out << target;
}

