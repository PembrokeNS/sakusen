#include "updatedunit.h"
#include "partialworld.h"

namespace sakusen {
namespace client {

/** Given information about a newly queued order, queues the order in
 * this.
 */
void UpdatedUnit::orderQueued(const OrderQueuedUpdateData& data)
{
  assert(data.getCondition() < orderCondition_max);
  orders.enqueueOrder(data.getCondition(), data.getOrder());
}

/** Given information about a queued order becoming current, accepts the
 * order.
 */
void UpdatedUnit::orderAccepted(const OrderAcceptedUpdateData& data)
{
  orders.acceptOrder(data.getCondition());
}

/** Given information about a completed order queue, clears the queue.
 */
void UpdatedUnit::orderCompleted(const OrderCompletedUpdateData& /*data*/)
{
  orders.clearQueue();
  orders.clearCurrent();
}

/** Updates the current status of the unit to \p to. */
void UpdatedUnit::alter(const CompleteUnit& to)
{
  CompleteUnit::operator=(to);
  altered = true;
}

/** Extrapolates the unit's new state from its current orders. */
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

