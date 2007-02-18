#include "layeredunit.h"

#include "completeworld.h"
#include "unitcore.h"
#include "region-methods.h"
#include "unitstatus-methods.h"

using namespace __gnu_cxx;

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
  Point<sint32> startPosition = startNear; /** \bug Find empty spot to start */
  Ptr unit(
      new LayeredUnit(type, startPosition, startOrientation, startVelocity)
    );
  unit->setRefToThis(Ref<LayeredUnit>(unit));
  world->addUnit(unit, owner);
}

void LayeredUnit::spawn(const PlayerID owner, const UnitTemplate& t)
{
  Ptr unit(new LayeredUnit(t));
  unit->setRefToThis(Ref<LayeredUnit>(unit));
  world->addUnit(unit, owner);
}

LayeredUnit::LayeredUnit(
    const UnitTemplate& t
  ) :
  owner(0),
  topLayer(new UnitCore(this, t.getStatus())),
  status(topLayer->getCore()),
  orders(t.getStatus()->getWeaponsStatus().size()),
  sensorReturns(10),
  dirty(false)
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
  status(topLayer->getCore()),
  orders(world->getUniverse()->getUnitTypePtr(startType)->getWeapons().size()),
  sensorReturns(10),
  dirty(false)
{
}

LayeredUnit::~LayeredUnit()
{
  /* Inform all returns from this unit that it is being removed */
  while (!sensorReturns.empty()) {
    sensorReturns.begin()->second->second->senseeDestroyed();
    sensorReturns.erase(sensorReturns.begin());
  }

  delete topLayer;
  topLayer = NULL;
  status = NULL;
}

void LayeredUnit::acceptOrder(OrderCondition condition)
{
  if (orders.getOrder(condition).isRealOrder()) {
    orders.acceptOrder(condition);

    /* Inform clients */
    world->getPlayerPtr(owner)->informClients(
        Update(new OrderAcceptedUpdateData(unitId, condition))
      );
  } else {
    /* This part of the function only meant for accepting new orders from
     * success or failure, not other conditions */
    assert(condition == orderCondition_lastOrderSuccess ||
        condition == orderCondition_lastOrderFailure);
    orders.clearQueue();
    orders.clearCurrent();
    world->getPlayerPtr(owner)->informClients(
        Update(new OrderCompletedUpdateData(unitId, condition))
      );
  }
}

void LayeredUnit::setDirty()
{
  /* The following algorithm assumes that dirty flags on sensor returns and
   * units are cleared at sufficiently close to the same time that it's safe to
   * assume that if we're still dirty, then so are they. */
  if (!dirty) {
    dirty = true;
    for (hash_map<PlayerID, DynamicSensorReturnsRef>::iterator returns =
        sensorReturns.begin(); returns != sensorReturns.end(); ++returns) {
      const Ref<DynamicSensorReturns>& r = returns->second->second;
      if (0 != (r->getPerception() & perception_unit)) {
        r->setDirty();
      }
    }
  }
}

void LayeredUnit::clearDirty()
{
  if (dirty) {
    dirty = false;
    world->getPlayerPtr(owner)->informClients(
        Update(new UnitAlteredUpdateData(getRefToThis()))
      );
  }
}

void LayeredUnit::setPosition(const Point<sint32>& pos)
{
  if (pos == status->position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(getRefToThis(), status->position, pos);
  status->position = pos;
}

void LayeredUnit::setPhysics(
      const Point<sint32>& newPosition,
      const Orientation& newOrientation,
      bool orientationIsRelative,
      bool zeroVelocity)
{
  setPosition(newPosition);
  if (orientationIsRelative) {
    status->orientation = newOrientation * status->orientation;
    if (zeroVelocity) {
      status->velocity.zero();
    } else {
      status->velocity = newOrientation * status->velocity;
    }
  } else {
    status->orientation = newOrientation;
    if (zeroVelocity) {
      status->velocity.zero();
    }
  }
  setDirty();
}

void LayeredUnit::incrementState(const Time& /*timeNow*/)
{
  /* TODO: rethink this function for subunits */

  /* Note that with this order system each unit can accept only one new order
   * per game cycle - if two arrive from the clients in the same game cycle
   * then things might get overwritten or lost - clients will need to be aware
   * of this and pay attention to the reports from the server about the unit's
   * orders' changes */

  /* If we've an order to apply right now, then do so */
  if (orders.getOrder(orderCondition_now).isRealOrder()) {
    acceptOrder(orderCondition_now);
  }

  Point<sint16> expectedVelocity(status->velocity);
  
  /* compute the expected velocity based on the unit's orders */
  switch (orders.getLinearTarget()) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity:
      if (topLayer->getPossibleVelocities()->contains(
            orders.getTargetVelocity()
          )) {
        Point<sint16> desiredVelocity = orders.getTargetVelocity();
        Point<sint16> acceleration = desiredVelocity - status->velocity;
        acceleration =
          topLayer->getPossibleAccelerations()->truncateToFit(acceleration);
        expectedVelocity += acceleration;
      } else {
        acceptOrder(orderCondition_lastOrderFailure);
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection = world->getMap()->getShortestDifference(
            orders.getTargetPosition(), status->position);
        Point<sint32> desiredVelocity =
          topLayer->getPossibleVelocities()->truncateToFit(desiredDirection);
        Point<sint16> acceleration = desiredVelocity - status->velocity;
        acceleration =
          topLayer->getPossibleAccelerations()->truncateToFit(acceleration);
        
        /*if (owner == 1 && unitId == 0) {
          Debug("[1] desiredVel=" << desiredVelocity <<
              ", acc=" << acceleration);
        }*/
        
        expectedVelocity += acceleration;
      }
      break;
    default:
      Fatal("Unknown linearTargetType '" << orders.getLinearTarget() << "'");
      break;
  }

  /* Update the expected velocity to take account of gravity and the ground. */
  /** \todo Replace this extremely crude collision detection with the ground
   * with something more sane */
  Box<sint32> boundingBox(getBoundingBox());
  sint32 groundHeight = world->getCompleteMap()->getHeightfield().
    getMaxHeightIn(boundingBox.rectangle());
  sint32 heightAboveGround = boundingBox.getMin().z - groundHeight;
  if (heightAboveGround + expectedVelocity.z < 0) {
    /* The unit will end up below the ground, so we need to raise it. */
    /** \bug The
     * way this is done at pesent is quite silly and could easily result in
     * exceptionally high velocities. (This is the same bug that lots of
     * speedruns exploit in other games to achieve high speeds) */
    expectedVelocity.z = -heightAboveGround;
  } else if (heightAboveGround > 0 && status->getTypePtr()->getGravity()) {
    /* The unit is in the air above the ground, so we need to apply gravity
     * */
    expectedVelocity.z -= world->getMap()->getGravity();
    /* But at the same time ensure that we don't push the unit underground */
    if (expectedVelocity.z < -heightAboveGround) {
      expectedVelocity.z = -heightAboveGround;
    }
  }

  /*if (owner == 1 && unitId == 0) {
    Debug("[2] heightAboveGround=" << heightAboveGround <<
        ", expectedVelocity=" << expectedVelocity);
  }*/

  /* Now set the velocity to this newly computed value */
  if (expectedVelocity != status->velocity) {
    status->velocity = expectedVelocity;
    setDirty();
  }
  
  /* TODO: do collision detection */
  Orientation mapOrientationChange;
  setPosition(world->getMap()->addToPosition(
        status->position, status->velocity, &mapOrientationChange
      ));
  /* If the movement caused us to rotate/reflect (due to moving over a map
   * edge) then update orientation and velocity appropriately */
  if (mapOrientationChange != Orientation()) {
    status->velocity = mapOrientationChange * status->velocity;
    status->orientation = mapOrientationChange * status->orientation;
  }

  /* Process the weapons */
  topLayer->incrementWeaponsState();
  
  /* determine if the currentOrder has succeeded or failed, and if so
   * then update the currentOrder appropriately and inform clients */
  switch (orders.getCurrentOrder().getType()) {
    case orderType_none:
      break;
    case orderType_setVelocity:
      if (status->velocity ==
          orders.getCurrentOrder().getSetVelocityData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    case orderType_move:
      if (status->position ==
          orders.getCurrentOrder().getMoveData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    case orderType_targetSensorReturns:
      /** \todo Check whether the SensorReturns still exists.  If not, then we
       * imagine that we have succeeded */
      break;
    case orderType_targetPoint:
      /* There's not really any sense in which this can succeed */
      break;
    default:
      Fatal("Unknown orderType '" <<
          orders.getCurrentOrder().getType() << "'");
      break;
  }
}

void LayeredUnit::enqueueOrder(
    const OrderCondition& condition,
    const Order& order
  ) {
  /*Debug("condition=" << condition);*/
  if (condition >= orderCondition_max || condition < 0) {
    Fatal("Unknown OrderCondition");
  }
  orders.enqueueOrder(condition, order);
  world->getPlayerPtr(owner)->informClients(
      Update(new OrderQueuedUpdateData(unitId, order, condition))
    );
  if (condition == orderCondition_incidental) {
    acceptOrder(orderCondition_incidental);
  }
}

bool LayeredUnit::setRadar(bool active) {
  if (topLayer->getVision().radarActive.capable) {
    status->radarIsActive = active;
    setDirty();
    return active;
  } else return false;
}

bool LayeredUnit::setSonar(bool active) {
  if (topLayer->getVision().sonarActive.capable) {
    status->sonarIsActive = active;
    setDirty();
    return active;
  } else return false;
}

}}//End Namespaces

