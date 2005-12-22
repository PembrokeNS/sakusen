#include "setvelocityorderdata.h"

OrderData* SetVelocityOrderData::newCopy(void) const
{
  return new SetVelocityOrderData(*this);
}

