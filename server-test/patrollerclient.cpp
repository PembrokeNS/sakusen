#include "patrollerclient.h"

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::test;

PatrollerClient::PatrollerClient(ClientID i, Point<sint32> p) :
  Client(i),
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
          patrolFrom.z = patrolTo.z;
          /*Debug("sending move order");*/
          QDebug("Patrolling to " << patrolTo);
          orderMessageQueue.push(
              OrderMessage(0 /* unit id */, orderCondition_now,
                Order(new MoveOrderData(patrolTo))
              )
            );
          headedOutward = true;
        }
      }
      break;
    case updateType_unitAltered:
      {
        /*const UnitAlteredUpdateData& data = update.getUnitAlteredData();
        QDebug("Unit at " << data.getUnit().getStatus().getPosition());*/
      }
      break;
    case updateType_sensorReturnsAdded:
      {
        const SensorReturnsAddedUpdateData& data =
          update.getSensorReturnsAddedData();
        otherUnit = new SensorReturns(data.getSensorReturns());
        QDebug("otherUnit at " <<
            otherUnit->getUnit()->getIStatus()->getPosition());
        /* throw in a weapon targeting orders */
        for (uint16 weaponIndex = 0; weaponIndex < 2; ++weaponIndex) {
          orderMessageQueue.push(
              OrderMessage(0 /* unit id */, orderCondition_incidental, Order(
                  new TargetPointOrderData(
                    weaponIndex,
                    otherUnit->getUnit()->getIStatus()->getPosition()
                  )
                ))
            );
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
                      Order(new MoveOrderData(patrolFrom))
                    )
                  );
                headedOutward = false;
              } else {
                orderMessageQueue.push(
                    OrderMessage(0 /* unit id */, orderCondition_now,
                      Order(new MoveOrderData(patrolTo))
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

