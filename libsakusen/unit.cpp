#include "unit.h"

#include "world.h"

using namespace sakusen;

void Unit::spawn(
    const PlayerID owner,
    const UnitTypeID type,
    const Point<sint32>& startNear,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity)
{
  Point<sint32> startPosition = startNear; /* FIXME: find empty spot to start */
  world->addUnit(
      Unit(type, startPosition, startOrientation, startVelocity),
      owner
    );
}

Unit::Unit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  owner(0),
  type(startType),
  unitId(0),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  initializeFromUnitType(
      world->getUniverse()->getUnitTypePtr(type),
      world->getUniverse());
  hitPoints = maxHitPoints; /* TODO: allow for fewer HP */
}

Unit::Unit(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    const Universe* universe
  ) :
  owner(0),
  type(startType),
  unitId(0),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  assert(world==NULL); /* This constructor is for when making maps, not when
                          playing the game */
  
  initializeFromUnitType(universe->getUnitTypePtr(type), universe);
  hitPoints = maxHitPoints; /* TODO: allow for fewer HP */
}

void Unit::initializeFromUnitType(
    const UnitType* typePtr,
    const Universe* universe)
{
  maxHitPoints = typePtr->getMaxHitPoints();
  mass = typePtr->getMass();
  size = typePtr->getSize();
  maxAcceleration = typePtr->getMaxAcceleration();
  maxMinusAcceleration = typePtr->getMaxMinusAcceleration();
  maxSpeed = typePtr->getMaxSpeed();
  maxMinusSpeed = typePtr->getMaxMinusSpeed();
  maxYawSpeed = typePtr->getMaxYawSpeed();
  maxPitchSpeed = typePtr->getMaxPitchSpeed();
  visibility = typePtr->getVisibility();
  vision = typePtr->getVision();
  
  /* TODO: add subunits if they normally come with this UnitType */
  
  /* add weapons */
  const std::list<WeaponTypeID>& weaponTypes = typePtr->getWeapons();
  for (std::list<WeaponTypeID>::const_iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    weapons.push_back(Weapon(universe->getWeaponTypePtr(*weaponType)));
  }
}

void Unit::acceptOrder(OrderCondition condition)
{
  if (orders[condition].isRealOrder()) {
    /* accept the order */
    currentOrder = orders[condition];
    
    /* Clear all orders from the queue */
    for (int i=0; i<orderCondition_max; i++) {
      orders[i] = Order();
    }

    /* Alter the Unit's state appropriately for the order */
    switch (currentOrder.getOrderType()) {
      case orderType_setVelocity:
        linearTarget = linearTargetType_velocity;
        targetVelocity = currentOrder.getSetVelocityData().getTarget();
        break;
      case orderType_move:
        linearTarget = linearTargetType_position;
        targetPosition = currentOrder.getMoveData().getTarget();
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
    currentOrder = Order();
    world->getPlayerPtr(owner)->informClients(
        Update(OrderCompletedUpdateData(unitId, condition))
      );
  }
}  

void Unit::setPosition(const Point<sint32> pos)
{
  if (pos == position)
    return;
  /* Whenever a unit position changes, we need to check
   * whether it has entered/exited the region of some effect */
  world->applyEntryExitEffects(*this, position, pos);
  position = pos;
}

void Unit::setPhysics(
      const Point<sint32>& newPosition,
      const Orientation& newOrientation,
      bool orientationIsRelative,
      bool zeroVelocity)
{
  setPosition(newPosition);
  if (orientationIsRelative) {
    setOrientation(newOrientation * orientation);
    if (zeroVelocity) {
      velocity.zero();
    } else {
      setVelocity(newOrientation * velocity);
    }
  } else {
    setOrientation(newOrientation);
    if (zeroVelocity) {
      velocity.zero();
    }
  }
}

void Unit::incrementState(const Time& /*timeNow*/)
{
  /* TODO: rethink this function for subunits */

  /* Note that with this order system each unit can accept only one new order
   * per game cycle - if two arrive from the clients in the same game cycle
   * then things might get overwritten or lost - clients will need to be aware
   * of this and pay attention to the reports from the server about the units
   * orders changes */

  /* If we've an order to apply right now, then do so */
  if (orders[orderCondition_now].isRealOrder()) {
    acceptOrder(orderCondition_now);
  }
  
  /* FIXME: Currently we use completely naive alterations to velocity: We allow
   * arbitrary acceleration and any velocity of modulus at most that of
   * maxSpeed */
  switch (linearTarget) {
    case linearTargetType_none:
      break;
    case linearTargetType_velocity:
      if (targetVelocity.squareLength() <= maxSpeed.squareLength()) {
        setVelocity(targetVelocity);
        /* TODO: inform clients */
      } else {
        acceptOrder(orderCondition_lastOrderFailure);
      }
      break;
    case linearTargetType_position:
      {
        Point<sint32> desiredDirection = world->getMap()->getShortestDifference(
            targetPosition, position);
        Point<sint32> desiredVelocity;
        if (desiredDirection.squareLength() <= maxSpeed.squareLength()) {
          desiredVelocity = desiredDirection;
        } else {
          desiredVelocity = (desiredDirection *
            (maxSpeed.length() / desiredDirection.length())).truncate32();
        }
        if (velocity == desiredVelocity) {
          break;
        }
        velocity = desiredVelocity;
        /* TODO: inform clients */
      }
      break;
    default:
      Fatal("Unknown linearTargetType '" << linearTarget << "'");
      break;
  }
  
  /* TODO: do collision detection */
  Orientation mapOrientationChange = Orientation();
  setPosition(world->getMap()->addToPosition(
        position, velocity, &mapOrientationChange
      ));
  /* If the movement caused us to rotate/reflect (due to moving over a map
   * edge) then update orientation and velocity appropriately */
  if (mapOrientationChange != Orientation()) {
    setVelocity(mapOrientationChange * velocity);
    setOrientation(mapOrientationChange * orientation);
  }
  
  /* determine if the currentOrder has succeeded or failed, and if so
   * then update the currentOrder appropriately and inform clients */
  switch (currentOrder.getOrderType()) {
    case orderType_none:
      break;
    case orderType_setVelocity:
      if (velocity == currentOrder.getSetVelocityData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    case orderType_move:
      if (position == currentOrder.getMoveData().getTarget()) {
        acceptOrder(orderCondition_lastOrderSuccess);
      }
      break;
    default:
      Fatal("Unknown orderType '" << currentOrder.getOrderType() << "'");
      break;
  }
  
  /* TODO: update player's worldview based on what this unit senses */
  /* TODO: update *other* players' worldviews (adding SensorReturn for this
   * unit) (unless this is done in World::incrementGameState) */
}

void Unit::enqueueOrder(const OrderCondition& condition, const Order& order) {
  if (condition >= orderCondition_max || condition < 0) {
    Fatal("Unknown OrderCondition");
  }
  orders[condition] = order;
  world->getPlayerPtr(owner)->informClients(
      Update(OrderQueuedUpdateData(unitId, &(orders[condition]), condition))
    );
}

void Unit::kill(HitPoints excessDamage) {
  /* do the destruct action */
  onDestruct();
  /* Change to corpse */
  changeType(type->getCorpseUnitType(), fullHitPoints);
  /* Apply excess damage to corpse */
  damage(excessDamage);
  /* TODO: deal with subunits */
}

void Unit::damage(HitPoints amount) {
  if (0 == amount) {
    return;
  }
  
  if (hitPoints <= amount) {
    kill(amount-hitPoints);
  } else {
    hitPoints -= amount;
  }
  /* TODO: inform clients */
}

void Unit::repair(HitPoints amount, bool superhealth) {
  if (0 == amount) {
    return;
  }
  
  if (superhealth) {
    hitPoints += amount;
  } else {
    if (hitPoints + amount > maxHitPoints)
      hitPoints = maxHitPoints;
    else
      hitPoints += amount;
  }
  /* TODO: inform clients */
}

void Unit::changeType(const UnitTypeID& to, hitPointAlteration hpAlteration) {
  const UnitType* newType;
  if ((newType = world->getUnitTypePtr(to)) == NULL) {
    Debug("[Unit::changeType] Invalid UnitTypeID");
  } else {
    switch (hpAlteration)
    {
      case scaleHitPoints:
        /* FIXME: The following calculation is sure to overflow */
        hitPoints = hitPoints * newType->getMaxHitPoints() / type->getMaxHitPoints();
        break;
      case fixHitPoints:
        hitPoints = std::min(hitPoints, newType->getMaxHitPoints());
        break;
      default:
        Debug("[Unit::changeType] Invalid hitpointAlteration");
      case fullHitPoints:
        hitPoints = newType->getMaxHitPoints();
        break;
    }
    maxHitPoints = newType->getMaxHitPoints();
    mass = newType->getMass();
    size = newType->getSize();
    maxAcceleration = newType->getMaxAcceleration();
    maxMinusAcceleration = newType->getMaxMinusAcceleration();
    maxSpeed = newType->getMaxSpeed();
    maxYawSpeed = newType->getMaxYawSpeed();
    maxPitchSpeed = newType->getMaxPitchSpeed();
    visibility = newType->getVisibility();
    type = to;

    /* TODO: check everything is within the new bounds (e.g. speed)
     * (unless the engine does that for us at next tick) */
    /* TODO: deal with subunits */
    /* TODO: inform clients */
  }
}

void Unit::changeOwner(PlayerID to, enum changeOwnerReason why) {
  /* existence check goes here */
  Player* fromPtr = world->getPlayerPtr(owner);
  Player* toPtr = world->getPlayerPtr(to);
  if (why != changeOwnerReason_created) {
    fromPtr->removeUnit(unitId, why);
  }
  owner = to;
  toPtr->addUnit(this, why);
  /* TODO: deal with subunits */
}

#if 0
void Unit::fire(Weapon* w, Unit* target) {
  /* TODO: implement target leading */
  fire(w, target.getPosition);
}

void Unit::fire(Weapon* w, SensorReturn* target) {
  /* TODO: implement target leading */
  fire(w, target.getBestPosition);
}

void Unit::fire(Weapon* w, Point<sint32> direction, uint16 speed) {
#warning write Unit::fire(Weapon*,Polar,uint16)
}

void Unit::fire(Weapon* w, Point<sint32> target) {
#warning write Unit::fire(Weapon* Point<uint32>)
}
#endif

bool Unit::setRadar(bool active) {
  if (vision.radarActive.capable) {
    radarIsActive = active;
    /* TODO: inform clients */
    return active;
  } else return false;
}

bool Unit::setSonar(bool active) {
  if (vision.sonarActive.capable) {
    sonarIsActive = active;
    /* TODO: inform clients */
    return active;
  } else return false;
}

void Unit::store(OArchive& archive) const
{
  /* TODO: See comments in Unit::load below.  The same applies here */
  
  archive << type->getInternalName()
    /* FIXME: assumes UnitTypeID is UnitType* */ << position;
  orientation.store(archive);
  archive << velocity;
}

Unit Unit::load(IArchive& archive, const Universe* universe)
{
  /* TODO: This function needs to behave differently according to the
   * circumstances under which it is called.  The current implementation is
   * suitable only for deserialization of a map file */

  /* TODO: At present only a bare minimum of functionality is implemented (just
   * enough to be able to call the world-less constructor).  We need to be able
   * to override each value which is copied across from UnitType */

  String typeName;
  archive >> typeName;
  UnitTypeID type = universe->getUnitTypeID(typeName);
  Point<sint32> position;
  archive >> position;
  Orientation orientation(Orientation::load(archive));
  Point<sint16> velocity;
  archive >> velocity;
  return Unit(type, position, orientation, velocity, universe);
}

