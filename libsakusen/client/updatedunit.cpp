#include "updatedunit.h"
#include "partialworld.h"

namespace sakusen {
namespace client {

void UpdatedUnit::orderQueued(const OrderQueuedUpdateData& data)
{
  assert(data.getCondition() < orderCondition_max);
  orders.enqueueOrder(data.getCondition(), data.getOrder());
}

void UpdatedUnit::orderAccepted(const OrderAcceptedUpdateData& data)
{
  orders.acceptOrder(data.getCondition());
}

void UpdatedUnit::orderCompleted(const OrderCompletedUpdateData& /*data*/)
{
  orders.clear();
}

void UpdatedUnit::alter(const CompleteUnit& to)
{
  *this = to;
  altered = true;
}

void UpdatedUnit::incrementState()
{
  if (altered) {
    altered = false;
  } else {
    Orientation mapOrientationChange;
    status.position = world->getMap()->addToPosition(
          status.position, status.velocity, &mapOrientationChange
        );
    /* If the movement caused us to rotate/reflect (due to moving over a map
     * edge) then update orientation and velocity appropriately */
    if (mapOrientationChange != Orientation::identity) {
      status.velocity = mapOrientationChange * status.velocity;
      status.orientation = mapOrientationChange * status.orientation;
    }
  }
}

}}

