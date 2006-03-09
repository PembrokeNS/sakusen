#include "updatedunit.h"

namespace sakusen {
namespace client {

void UpdatedUnit::clearOrders()
{
  for (int i=0; i<orderCondition_max; ++i) {
    status.orders[i] = Order();
  }
}

void UpdatedUnit::orderQueued(const OrderQueuedUpdateData& data)
{
  assert(data.getCondition() < orderCondition_max);
  status.orders[data.getCondition()] = data.getOrder();
}

void UpdatedUnit::orderAccepted(const OrderAcceptedUpdateData& data)
{
  assert(data.getCondition() < orderCondition_max);
  status.currentOrder = status.orders[data.getCondition()];
  assert(status.currentOrder.isRealOrder());
  clearOrders();
}

void UpdatedUnit::orderCompleted(const OrderCompletedUpdateData& data)
{
  assert(data.getCondition() < orderCondition_max);
  status.currentOrder = Order();
  clearOrders();
}

void UpdatedUnit::incrementState()
{
  if (updated) {
    updated = false;
  } else {
    status.position += status.velocity;
  }
}

}}

