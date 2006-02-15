#include "patrollerclient.h"
#include "unit.h"

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::test;

PatrollerClient::PatrollerClient(Point<sint32> p) :
  patrolTo(p),
  patrolFrom(),
  headedOutward(false)
{
}

void PatrollerClient::sendUpdate(const Update& update)
{
  switch (update.getType()) {
    case updateType_unitAdded:
      {
        const UnitAddedUpdateData& data = update.getUnitAddedData();
        if (data.getUnit().getId() == 0 &&
            data.getReason() == changeOwnerReason_created) {
          patrolFrom = data.getUnit().getPosition();
          /*Debug("sending move order");*/
          orderMessageQueue.push(
              OrderMessage(0 /* unit id */, orderCondition_now,
                Order(MoveOrderData(patrolTo))
              )
            );
          headedOutward = true;
        }
      }
      break;
    case updateType_orderCompleted:
      {
        const OrderCompletedUpdateData& data = update.getOrderCompletedData();
        if (data.getUnitId() == 0) {
          switch (data.getCondition()) {
            case orderCondition_lastOrderSuccess:
              if (headedOutward) {
                orderMessageQueue.push(
                    OrderMessage(0 /* unit id */, orderCondition_now,
                      Order(MoveOrderData(patrolFrom))
                    )
                  );
                headedOutward = false;
              } else {
                orderMessageQueue.push(
                    OrderMessage(0 /* unit id */, orderCondition_now,
                      Order(MoveOrderData(patrolTo))
                    )
                  );
                headedOutward = true;
              }
              break;
            case orderCondition_lastOrderFailure:
              break;
            default:
              break;
          }
        }
      }
      break;
    default:
      break;
  }
}

