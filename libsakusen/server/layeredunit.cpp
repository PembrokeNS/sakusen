#include "layeredunit.h"

#include "completeworld.h"
#include "unitcore.h"
#include "unitmask.h"
#include "region-methods.h"
#include "unitstatus-methods.h"

using namespace __gnu_cxx;

namespace sakusen{
namespace server{

/** \brief Spawn a unit on the map, specifying the hp. */
Ref<LayeredUnit> LayeredUnit::spawn(
    const PlayerId owner,
    const UnitTypeId type,
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

/** \brief Spawn a unit on the map, with default hp.
 *
 * This is the most likely method to call from module code when creating a
 * unit.  The other overload may also be appropriate. */
Ref<LayeredUnit> LayeredUnit::spawn(
    const PlayerId owner,
    const UnitTypeId type,
    const Point<sint32>& startNear,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  )
{
  assert(type);
  Point<sint32> startPosition = startNear; /** \bug Find empty spot to start */
  Ptr unit(new LayeredUnit(
        type, startPosition, startOrientation, startVelocity
      ));
  return world->addUnit(unit, owner);
}

/** \brief Spawn a unit on the map according to a template.
 *
 * This is primarily intended for use during initial population of the map, but
 * may also be used at other times. */
Ref<LayeredUnit> LayeredUnit::spawn(
    const PlayerId owner,
    const UnitTemplate& t
  )
{
  Ptr unit(new LayeredUnit(t));
  return world->addUnit(unit, owner);
}

LayeredUnit::LayeredUnit(
    const UnitTemplate& t
  ) :
  owner(),
  topLayer(new UnitCore(this, t.getStatus())),
  status(topLayer->getCore()),
  motion(UnitMotion::create(t.getStatus().getType()->getMotionType())),
  orders(static_cast<uint16>(t.getStatus().getWeaponsStatus().size())),
  sensorReturns(10),
  dirty(false)
{
}

LayeredUnit::LayeredUnit(
    const UnitTypeId& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHP
  ) :
  owner(),
  topLayer(new UnitCore(
        this, startType, startPosition, startOrientation, startVelocity,
        startHP
      )),
  status(topLayer->getCore()),
  motion(UnitMotion::create(startType->getMotionType())),
  orders(static_cast<uint16>(world->getUniverse()->getUnitTypePtr(startType)->getWeapons().size())),
  sensorReturns(10),
  dirty(false)
{
}

LayeredUnit::LayeredUnit(
    const UnitTypeId& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  owner(),
  topLayer(new UnitCore(
        this, startType, startPosition, startOrientation, startVelocity
      )),
  status(topLayer->getCore()),
  motion(UnitMotion::create(startType->getMotionType())),
  orders(static_cast<uint16>(world->getUniverse()->getUnitTypePtr(startType)->getWeapons().size())),
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
    for (hash_map<PlayerId, DynamicSensorReturnsRef>::iterator returns =
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
  if (pos == status.position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(getRefToThis(), status.position, pos);
  status.position = pos;
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
    status.orientation = newOrientation * status.orientation;
    if (zeroVelocity) {
      status.velocity.zero();
    } else {
      status.velocity = newOrientation * status.velocity;
    }
  } else {
    status.orientation = newOrientation;
    if (zeroVelocity) {
      status.velocity.zero();
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

  motion->incrementState(*this);

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
    status.radarIsActive = active;
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
    status.sonarIsActive = active;
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

