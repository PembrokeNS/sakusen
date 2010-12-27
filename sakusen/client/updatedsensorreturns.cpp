#include <sakusen/client/updatedsensorreturns.h>

#include <sakusen/client/partialworld.h>

namespace sakusen {
namespace client {

Ref<ISensorReturns> UpdatedSensorReturns::getRefToThis()
{
  return world->getSensorReturnsById()->find(this->getId());
}

/** \brief Alter this to mimic the given SensorReturns
 *
 * \warning Don't try to achieve such an alteration with simple assignment.
 * It will go horribly wrong.
 */
void UpdatedSensorReturns::alter(const SensorReturns& to)
{
  this->SensorReturns::operator=(to);
  altered_ = true;
  altered();
}

void UpdatedSensorReturns::incrementState()
{
  if (altered_) {
    altered_ = false;
  } else {
    if (!unit) {
      SAKUSEN_FATAL("Shouldn't fail to alter non-unit return (perception = " << getPerception() << ", time=" << world->getTimeNow() << ")");
    }
    UnitStatus& status = unit->getStatus();
    world->getMap()->transform(
          status.frame, status.velocity, status.angularVelocity
        );
    altered();
  }
}

}
}

