#include "layeredunit.h"

#include "completeworld.h"
#include "unitcore.h"
#include "unitmask.h"
#include "region-methods.h"
#include "unitstatus-methods.h"

using namespace __gnu_cxx;

namespace sakusen{
namespace server{

/** \brief Spawn a unit on the map.
 *
 * This is the most likely method to call from module code when creating a
 * unit.  The other overload may also be appropriate. */
Ref<LayeredUnit> LayeredUnit::spawn(
    const PlayerID owner,
    const UnitTypeID type,
    const Point<sint32>& startNear,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  )
{
  assert(type);
  Point<sint32> startPosition = startNear; /** \bug Find empty spot to start */
  Ptr unit(new LayeredUnit(
        type, startPosition, startOrientation, startVelocity, startHP
      ));
  return world->addUnit(unit, owner);
}

/** \brief Spawn a unit on the map according to a template.
 *
 * This is primarily intended for use during initial population of the map, but
 * may also be used at other times. */
Ref<LayeredUnit> LayeredUnit::spawn(
    const PlayerID owner,
    const UnitTemplate& t
  )
{
  Ptr unit(new LayeredUnit(t));
  return world->addUnit(unit, owner);
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
    const Point<sint16>& startVelocity,
    HitPoints startHP
  ) :
  owner(0),
  topLayer(new UnitCore(
        this, startType, startPosition, startOrientation, startVelocity,
        startHP
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

  status = NULL;
  assert(topLayer.unique());
}

void LayeredUnit::acceptOrder(const Order& order)
{
  orders.acceptOrder(order);

  /* Inform clients */
  world->getPlayerPtr(owner)->informClients(
      Update(new OrderAcceptedUpdateData(unitId, order))
    );
}

/** \brief Indicate that a change has occured that needs to be transmitted to
 * clients.
 *
 * This sets not only the dirty flag on this unit, but also on every
 * SensorReturns from this unit. */
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

/** \brief Clear dirty flag, transmitting updates as necessary.
 *
 * This ensures that the dirty flag for this unit is clear.  If it was not
 * already clear, then it transmits a UnitAlteredUpdate to its players clients.
 */
void LayeredUnit::clearDirty()
{
  if (dirty) {
    dirty = false;
    world->getPlayerPtr(owner)->informClients(
        Update(new UnitAlteredUpdateData(getRefToThis()))
      );
  }
}

Ref<LayeredUnit> LayeredUnit::getRefToThis()
{
  hash_list<LayeredUnit, Bounded>& units = world->getUnits();
  hash_list<LayeredUnit, Bounded>::iterator thisIt = units.find(this);
  assert(thisIt != units.end());
  return *thisIt;
}

Ref<const LayeredUnit> LayeredUnit::getRefToThis() const
{
  return *world->getUnits().find(this);
}

/** \brief Alters the unit's position, ensuring all appropriate action caused
 * by this change is taken. */
void LayeredUnit::setPosition(const Point<sint32>& pos)
{
  if (pos == status->position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(getRefToThis(), status->position, pos);
  status->position = pos;
}

/** \brief Allow simultaneous update of the unit's position, orientation and
 * velocity.
 *
 * \param newPosition           New position for the unit.
 * \param newOrientation        New orientation of the unit, or transformation
 *                              to be performed to the unit's orientation,
 *                              according to the value of
 *                              orientationIsRelative.
 * \param orientationIsRelative If true, the unit's orientation is
 *                              premultiplied by newOrientation.  If false, the
 *                              unit's orientation is set to newOrientation.
 * \param zeroVelocity          If true, the unit's velocity is set to zero.
 *                              If false, the unit's velocity is unchanged.
 *
 * The intended purpose of this function is to allow easy implementation of
 * things such as teleportation from module code.
 */
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

/** \brief Update the unit's state for the new tick.
 *
 * This function handles all the unit's order interpretation, acceleration and
 * weapons fire.  Any changes which require it will cause the dirty flag to be
 * set.
 */
void LayeredUnit::incrementState(const Time& /*timeNow*/)
{
  /** \todo Rethink this function for subunits */

  /* Note that with this order system each unit can accept only one new order
   * per game cycle - if two arrive from the clients in the same game cycle
   * then things might get overwritten or lost - clients will need to be aware
   * of this and pay attention to the reports from the server about the unit's
   * orders' changes */

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
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection = world->getMap()->getShortestDifference(
            orders.getTargetPosition(), status->position);
        Point<sint16> desiredVelocity(
            topLayer->getPossibleVelocities()->truncateToFit(desiredDirection)
          );
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
  
  /** \todo Do collision detection. */
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
}

/** \brief Inserts a new layer at the top of the unit's layers */
void LayeredUnit::insertLayer(const sakusen::server::UnitMask::Ptr& layer)
{
  layer->nextLayer = topLayer;
  topLayer = layer;
}

/** \brief Set radar status.
 *
 * Attempts to set the state of the unit's radar (active/inactive) according to
 * \p active.  Return value indicates state after the call; if not equal to
 * active then the unit doesn't support radar.
 */
bool LayeredUnit::setRadar(bool active) {
  if (topLayer->getVision().radarActive.capable) {
    status->radarIsActive = active;
    setDirty();
    return active;
  } else return false;
}

/** \brief Set sonar status.
 *
 * Attempts to set the state of the unit's sonar (active/inactive) according to
 * \p active.  Return value indicates state after the call; if not equal to
 * active then the unit doesn't support sonar.
 */
bool LayeredUnit::setSonar(bool active) {
  if (topLayer->getVision().sonarActive.capable) {
    status->sonarIsActive = active;
    setDirty();
    return active;
  } else return false;
}

/** \brief Removes a layer from amongst the unit's layers
 *
 * Fatals if that layer does not exist */
void LayeredUnit::removeLayer(const UnitMask* layer) {
  if (topLayer.get() == layer) {
    topLayer = layer->nextLayer;
  } else {
    topLayer->removeLayer(layer);
  }
  setDirty();
}

}}//End Namespaces

