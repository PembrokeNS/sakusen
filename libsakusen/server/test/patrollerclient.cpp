#include "patrollerclient.h"

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::test;

PatrollerClient::PatrollerClient(Point<sint32> p) :
  patrolTo(p),
  patrolFrom(),
  headedOutward(false),
  otherUnit(NULL)
{
}

void PatrollerClient::queueUpdate(const Update& update)
{
  switch (update.getType()) {
    case updateType_unitAdded:
      {
        const UnitAddedUpdateData& data = update.getUnitAddedData();
        if (data.getUnit().getId() == 0 &&
            data.getReason() == changeOwnerReason_created) {
          patrolFrom = data.getUnit().getStatus().getPosition();
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
    case updateType_sensorReturnsAdded:
      {
        const SensorReturnsAddedUpdateData& data =
          update.getSensorReturnsAddedData();
        otherUnit = new SensorReturns(data.getSensorReturns());
        /* throw in a weapon targeting order */
        orderMessageQueue.push(
            OrderMessage(0 /* unit id */, orderCondition_incidental, Order(
                TargetSensorReturnsOrderData(
                  0 /* weapon index */, otherUnit
                )
              ))
          );
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

