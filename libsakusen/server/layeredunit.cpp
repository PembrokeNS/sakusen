#include "layeredunit.h"

#include "completeworld.h"
#include "unitcore.h"
#include "region-methods.h"
#include "unitstatus-methods.h"

namespace sakusen{
namespace server{

void LayeredUnit::spawn(
    const PlayerID owner,
    const UnitTypeID type,
    const Point<sint32>& startNear,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  )
{
  Point<sint32> startPosition = startNear; /* FIXME: find empty spot to start */
  world->addUnit(
      LayeredUnit(type, startPosition, startOrientation, startVelocity),
      owner
    );
}

void LayeredUnit::spawn(const PlayerID owner, const UnitTemplate& t)
{
  world->addUnit(LayeredUnit(t), owner);
}

LayeredUnit::LayeredUnit(
    const UnitTemplate& t
  ) :
  owner(0),
  topLayer(new UnitCore(this, t.getStatus())),
  unit(topLayer->getCore())
{
}

LayeredUnit::LayeredUnit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  owner(0),
  topLayer(new UnitCore(
        this, startType, startPosition, startOrientation, startVelocity
      )),
  unit(topLayer->getCore())
{
}

LayeredUnit::LayeredUnit(const LayeredUnit& copy) :
  ICompleteUnit(copy),
  owner(copy.owner),
  topLayer(copy.topLayer->newCopy(this)),
  unit(topLayer->getCore())
{
}

LayeredUnit& LayeredUnit::operator=(const LayeredUnit& copy)
{
  delete topLayer;
  ICompleteUnit::operator=(copy);
  topLayer = copy.topLayer->newCopy(this);
  unit = topLayer->getCore();

  return *this;
}

LayeredUnit::~LayeredUnit()
{
  delete topLayer;
  topLayer = NULL;
}

void LayeredUnit::acceptOrder(OrderCondition condition)
{
  Order newOrder = unit->orders[condition];
  if (newOrder.isRealOrder()) {
    /* accept the order */
    unit->currentOrder = newOrder;
    
    /* Clear all orders from the queue */
    for (int i=0; i<orderCondition_max; i++) {
      unit->orders[i] = Order();
    }

    /* Alter the Unit's state appropriately for the order */
    switch (newOrder.getOrderType()) {
      case orderType_setVelocity:
        unit->linearTarget = linearTargetType_velocity;
        unit->targetVelocity = newOrder.getSetVelocityData().getTarget();
        break;
      case orderType_move:
        unit->linearTarget = linearTargetType_position;
        unit->targetPosition = newOrder.getMoveData().getTarget();
        break;
      default:
        Fatal("Unknown OrderType");
    }

    /* Inform clients */
    world->getPlayerPtr(owner)->informClients(
        Update(OrderAcceptedUpdateData(unitId, condition))
      );
  } else {
    /* This part of the function only meant for accepting new orders from
     * success or failure, not other conditions */
    assert(condition == orderCondition_lastOrderSuccess ||
        condition == orderCondition_lastOrderFailure);
    unit->currentOrder = Order();
    world->getPlayerPtr(owner)->informClients(
        Update(OrderCompletedUpdateData(unitId, condition))
      );
  }
}  

void LayeredUnit::setPosition(const Point<sint32>& pos)
{
  if (pos == unit->position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(*this, unit->position, pos);
  unit->position = pos;
}

void LayeredUnit::setPhysics(
      const Point<sint32>& newPosition,
      const Orientation& newOrientation,
      bool orientationIsRelative,
      bool zeroVelocity)
{
  setPosition(newPosition);
  if (orientationIsRelative) {
    unit->orientation = newOrientation * unit->orientation;
    if (zeroVelocity) {
      unit->velocity.zero();
    } else {
      unit->velocity = newOrientation * unit->velocity;
    }
  } else {
    unit->orientation = newOrientation;
    if (zeroVelocity) {
      unit->velocity.zero();
    }
  }
}

void LayeredUnit::incrementState(const Time& /*timeNow*/)
{
  /* TODO: rethink this function for subunits */

  /* Note that with this order system each unit can accept only one new order
   * per game cycle - if two arrive from the clients in the same game cycle
   * then things might get overwritten or lost - clients will need to be aware
   * of this and pay attention to the reports from the server about the units
   * orders changes */

  /* If we've an order to apply right now, then do so */
  if (unit->orders[orderCondition_now].isRealOrder()) {
    acceptOrder(orderCondition_now);
  }
  
  /* FIXME: Currently we use completely naive alterations to velocity: We allow
   * arbitrary acceleration and any velocity of modulus at most that of
   * maxSpeed */
  switch (unit->linearTarget) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity:
      if (topLayer->getPossibleVelocities().contains(unit->targetVelocity)) {
        unit->velocity = unit->targetVelocity;
        /* TODO: inform clients */
      } else {
        acceptOrder(orderCondition_lastOrderFailure);
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection = world->getMap()->getShortestDifference(
            unit->targetPosition, unit->position);
        Point<sint32> desiredVelocity;
        desiredVelocity =
          topLayer->getPossibleVelocities().truncateToFit(desiredDirection);
        if (unit->velocity == desiredVelocity) {
          break;
        }
        unit->velocity = desiredVelocity;
        /* TODO: inform clients */
      }
      break;
    default:
      Fatal("Unknown linearTargetType '" << unit->linearTarget << "'");
      break;
  }
  
  /* TODO: do collision detection */
  Orientation mapOrientationChange = Orientation();
  setPosition(world->getMap()->addToPosition(
        unit->position, unit->velocity, &mapOrientationChange
      ));
  /* If the movement caused us to rotate/reflect (due to moving over a map
   * edge) then update orientation and velocity appropriately */
  if (mapOrientationChange != Orientation()) {
    unit->velocity = mapOrientationChange * unit->velocity;
    unit->orientation = mapOrientationChange * unit->orientation;
  }
  
  /* determine if the currentOrder has succeeded or failed, and if so
   * then update the currentOrder appropriately and inform clients */
  switch (unit->currentOrder.getOrderType()) {
    case orderType_none:
      break;
    case orderType_setVelocity:
      if (unit->velocity ==
          unit->currentOrder.getSetVelocityData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    case orderType_move:
      if (unit->position ==
          unit->currentOrder.getMoveData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    default:
      Fatal("Unknown orderType '" <<
          unit->currentOrder.getOrderType() << "'");
      break;
  }
  
  /* TODO: update player's worldview based on what this unit senses */
  /* TODO: update *other* players' worldviews (adding SensorReturn for this
   * unit) (unless this is done in World::incrementGameState) */
}

void LayeredUnit::enqueueOrder(
    const OrderCondition& condition,
    const Order& order
  ) {
  /*Debug("condition=" << condition);*/
  if (condition >= orderCondition_max || condition < 0) {
    Fatal("Unknown OrderCondition");
  }
  unit->orders[condition] = order;
  world->getPlayerPtr(owner)->informClients(
      Update(OrderQueuedUpdateData(unitId, order, condition))
    );
}

bool LayeredUnit::setRadar(bool active) {
  if (topLayer->getVision().radarActive.capable) {
    unit->radarIsActive = active;
    /* TODO: inform clients */
    return active;
  } else return false;
}

bool LayeredUnit::setSonar(bool active) {
  if (topLayer->getVision().sonarActive.capable) {
    unit->sonarIsActive = active;
    /* TODO: inform clients */
    return active;
  } else return false;
}

}
}//End Namespaces

