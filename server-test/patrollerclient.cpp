#include "patrollerclient.h"

#define CLOSE_ENOUGH 1000

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::test;

PatrollerClient::PatrollerClient(ClientId i, Point<sint32> p) :
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
              OrderMessage(UnitId(),
                Order(new MoveOrderData(patrolTo))
              )
            );
          headedOutward = true;
        }
      }
      break;
    case updateType_unitAltered:
      {
        const UnitAlteredUpdateData& data = update.getUnitAlteredData();
        Position position = data.getUnit().getStatus().getPosition();
        /*QDebug("Unit at " << data.getUnit().getStatus().getPosition());*/

        if (headedOutward && (position - patrolTo).length() < CLOSE_ENOUGH) {
          orderMessageQueue.push(
              OrderMessage(UnitId(),
                Order(new MoveOrderData(patrolFrom))
              )
            );
          headedOutward = false;
        } else if (
            !headedOutward && (position - patrolFrom).length() < CLOSE_ENOUGH
          ) {
          orderMessageQueue.push(
              OrderMessage(UnitId(),
                Order(new MoveOrderData(patrolTo))
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
        QDebug("otherUnit at " <<
            otherUnit->getUnit()->getIStatus()->getPosition());
        /* throw in a weapon targeting orders */
        for (uint16 weaponIndex = 0; weaponIndex < 2; ++weaponIndex) {
          orderMessageQueue.push(
              OrderMessage(UnitId(), Order(
                  new TargetPositionOrderData(
                    weaponIndex,
                    otherUnit->getUnit()->getIStatus()->getPosition()
                  )
                ))
            );
        }
      }
      break;
    default:
      break;
  }
}

