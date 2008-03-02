#include "updatedunit.h"
#include "partialworld.h"
#include "heightfield-methods.h"

namespace sakusen {
namespace client {

/** Given information about a queued order becoming current, accepts the
 * order.
 */
void UpdatedUnit::orderAccepted(const OrderAcceptedUpdateData& data)
{
  orders.acceptOrder(data.getOrder());
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
    world->getMap()->transform(
          status.frame, status.velocity, status.angularVelocity
        );
  }
}

}}

