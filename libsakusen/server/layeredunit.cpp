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
    const Frame& startNear,
    const Point<sint16>& startVelocity,
    const HitPoints startHP
  )
{
  assert(type);
  Frame startFrame = startNear; /** \bug Find empty spot to start */
  Ptr unit(new LayeredUnit(
        type, startFrame, startVelocity, startHP
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
    const Frame& startNear,
    const Point<sint16>& startVelocity
  )
{
  assert(type);
  Frame startFrame = startNear; /** \bug Find empty spot to start */
  Ptr unit(new LayeredUnit(
        type, startFrame, startVelocity
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
    const Frame& startFrame,
    const Point<sint16>& startVelocity,
    HitPoints startHP
  ) :
  owner(),
  topLayer(new UnitCore(
        this, startType, startFrame, startVelocity,
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
    const Frame& startFrame,
    const Point<sint16>& startVelocity
  ) :
  owner(),
  topLayer(new UnitCore(
        this, startType, startFrame, startVelocity
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
    (*sensorReturns.begin()->second)->senseeDestroyed();
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
      const Ref<DynamicSensorReturns>& r = *returns->second;
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
void LayeredUnit::setPosition(const Point<sint32>& newPos)
{
  Position& position = status.getFrame().getPosition();
  if (newPos == position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(getRefToThis(), position, newPos);
  position = newPos;
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

  /* Take a reference so that we can detect ourselves being killed */
  Ref<LayeredUnit> refToThis = getRefToThis();
  
  /* Allow layers to do every-tick stuff (e.g. generate some resources) */
  topLayer->incrementState();
  if (!refToThis) { return; }

  /* Call motion handler to set velocity properly */
  motion->incrementState(*this);
  if (!refToThis) { return; }
  
  /* Do the actual move */
  Position oldPos = status.frame.getPosition();
  world->getMap()->transform(
      status.frame, status.velocity, status.angularVelocity
    );

  if (status.frame.getPosition() != oldPos) {
    /* Whenever a unit position changes, we need to check
     * whether it has entered/exited the region of some effect */
    world->applyEntryExitEffects(
        getRefToThis(), oldPos, status.frame.getPosition()
      );
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

