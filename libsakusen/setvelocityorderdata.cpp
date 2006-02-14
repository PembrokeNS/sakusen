#include "setvelocityorderdata.h"

using namespace sakusen;

OrderData* SetVelocityOrderData::newCopy(void) const
{
  return new SetVelocityOrderData(*this);
}

void SetVelocityOrderData::store(OArchive& out) const
{
  out << target;
}

