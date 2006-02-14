#include "layeredunit.h"

#include "completeworld.h"
#include "unitcore.h"
#include "unit-methods.h"
#include "region-methods.h"

using namespace sakusen::server;

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
  world->addUnit(LayeredUnit(
        t.getType(), t.getPosition(), t.getOrientation(), t.getVelocity(),
        t.getHitPoints(), t.isRadarActive(), t.isSonarActive()
      ), owner);
}

LayeredUnit::LayeredUnit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive
  ) :
  owner(0),
  topLayer(new UnitCore(
        this, startType, startPosition, startOrientation, startVelocity,
        startHitPoints, startRadarActive, startSonarActive
      )),
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
  IUnit(copy),
  owner(copy.owner),
  topLayer(copy.topLayer->newCopy(this)),
  unit(topLayer->getCore())
{
}

LayeredUnit::~LayeredUnit()
{
  delete topLayer;
  topLayer = NULL;
}

void LayeredUnit::acceptOrder(OrderCondition condition)
{
  Order newOrder = getOrder(condition);
  if (newOrder.isRealOrder()) {
    /* accept the order */
    unit->setCurrentOrder(newOrder);
    
    /* Clear all orders from the queue */
    for (int i=0; i<orderCondition_max; i++) {
      unit->setOrder(static_cast<OrderCondition>(i), Order());
    }

    /* Alter the Unit's state appropriately for the order */
    switch (newOrder.getOrderType()) {
      case orderType_setVelocity:
        unit->setLinearTarget(linearTargetType_velocity);
        unit->setTargetVelocity(newOrder.getSetVelocityData().getTarget());
        break;
      case orderType_move:
        unit->setLinearTarget(linearTargetType_position);
        unit->setTargetPosition(newOrder.getMoveData().getTarget());
        break;
      default:
        Fatal("Unknown OrderType");
    }

    /* Inform clients */
    world->getPlayerPtr(owner)->informClients(
        Update(OrderAcceptedUpdateData(getId(), condition))
      );
  } else {
    /* This part of the function only meant for accepting new orders from
     * success or failure, not other conditions */
    assert(condition == orderCondition_lastOrderSuccess ||
        condition == orderCondition_lastOrderFailure);
    unit->setCurrentOrder(Order());
    world->getPlayerPtr(owner)->informClients(
        Update(OrderCompletedUpdateData(getId(), condition))
      );
  }
}  

void LayeredUnit::setPosition(const Point<sint32>& pos)
{
  Point<sint32> oldPos = getPosition();
  if (pos == oldPos)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(*this, oldPos, pos);
  unit->setPosition(pos);
}

void LayeredUnit::setPhysics(
      const Point<sint32>& newPosition,
      const Orientation& newOrientation,
      bool orientationIsRelative,
      bool zeroVelocity)
{
  setPosition(newPosition);
  if (orientationIsRelative) {
    unit->setOrientation(newOrientation * getOrientation());
    if (zeroVelocity) {
      unit->zeroVelocity();
    } else {
      unit->setVelocity(newOrientation * getVelocity());
    }
  } else {
    unit->setOrientation(newOrientation);
    if (zeroVelocity) {
      unit->zeroVelocity();
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
  if (getOrder(orderCondition_now).isRealOrder()) {
    acceptOrder(orderCondition_now);
  }
  
  /* FIXME: Currently we use completely naive alterations to velocity: We allow
   * arbitrary acceleration and any velocity of modulus at most that of
   * maxSpeed */
  switch (getLinearTarget()) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity:
      if (getPossibleVelocities().contains(getTargetVelocity())) {
        unit->setVelocity(getTargetVelocity());
        /* TODO: inform clients */
      } else {
        acceptOrder(orderCondition_lastOrderFailure);
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection = world->getMap()->getShortestDifference(
            getTargetPosition(), getPosition());
        Point<sint32> desiredVelocity;
        desiredVelocity =
          getPossibleVelocities().truncateToFit(desiredDirection);
        if (getVelocity() == desiredVelocity) {
          break;
        }
        unit->setVelocity(desiredVelocity);
        /* TODO: inform clients */
      }
      break;
    default:
      Fatal("Unknown linearTargetType '" << getLinearTarget() << "'");
      break;
  }
  
  /* TODO: do collision detection */
  Orientation mapOrientationChange = Orientation();
  setPosition(world->getMap()->addToPosition(
        getPosition(), getVelocity(), &mapOrientationChange
      ));
  /* If the movement caused us to rotate/reflect (due to moving over a map
   * edge) then update orientation and velocity appropriately */
  if (mapOrientationChange != Orientation()) {
    unit->setVelocity(mapOrientationChange * getVelocity());
    unit->setOrientation(mapOrientationChange * getOrientation());
  }
  
  /* determine if the currentOrder has succeeded or failed, and if so
   * then update the currentOrder appropriately and inform clients */
  switch (getCurrentOrder().getOrderType()) {
    case orderType_none:
      break;
    case orderType_setVelocity:
      if (getVelocity() ==
          getCurrentOrder().getSetVelocityData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    case orderType_move:
      if (getPosition() ==
          getCurrentOrder().getMoveData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    default:
      Fatal("Unknown orderType '" <<
          getCurrentOrder().getOrderType() << "'");
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
  unit->setOrder(condition, order);
  world->getPlayerPtr(owner)->informClients(
      Update(OrderQueuedUpdateData(getId(), &order, condition))
    );
}

bool LayeredUnit::setRadar(bool active) {
  if (getVision().radarActive.capable) {
    unit->setRadarActive(active);
    /* TODO: inform clients */
    return active;
  } else return false;
}

bool LayeredUnit::setSonar(bool active) {
  if (getVision().sonarActive.capable) {
    unit->setSonarActive(active);
    /* TODO: inform clients */
    return active;
  } else return false;
}

