#include "updatedsensorreturns.h"

#include "partialworld.h"

namespace sakusen {
namespace client {

Ref<ISensorReturns> UpdatedSensorReturns::getRefToThis()
{
  return world->getSensorReturnsById()->find(this->getId());
}

}
}

