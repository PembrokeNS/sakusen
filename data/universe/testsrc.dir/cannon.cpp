#include "cannon.h"
#include "completeworld.h"

#include <iostream>

using namespace std;
using namespace sakusen;
using namespace sakusen::server;
using namespace testsrc;

bool Creater::aim(
    const Ref<LayeredUnit>& source,
    WeaponStatus* status,
    const WeaponOrders& orders
  )
{
  Position displacement =
    orders.getTargetPosition() - source->getStatus()->getPosition();
  if (displacement.squareLength() <= squareRange()) {
    status->setTarget(orders.getTargetPosition(), orders.getTargetOrientation());
    return true;
  }
  return false;
}

void Creater::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  switch (status.getTargetType()) {
    case weaponTargetType_positionOrientation:
      Debug("I'm trying to create a unit!!!");
      break;
    default:
      break;
  }
}

bool Builder::aim(
    const Ref<LayeredUnit>& source,
    WeaponStatus* status,
    const WeaponOrders& orders
  )
{
  if (orders.getTargetType() != weaponTargetType_unit) {
    return false;
  }
  Ref<LayeredUnit> target = orders.getTargetUnit().dynamicCast<LayeredUnit>();
  Position displacement =
    target->getStatus()->getPosition() - source->getStatus()->getPosition();
  if (displacement.squareLength() <= squareRange()) {
    /** \todo Find and set the appropriate building layer */
    setTarget(target, status);
    return true;
  }
  return false;
}

void Builder::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  switch (status.getTargetType()) {
    case weaponTargetType_unit:
      {
        Ref<const LayeredUnit> targetUnit = getTargetUnit();
        BuildingLayer::Ptr bL;
        if (!buildingLayer.expired()) {
          bL = buildingLayer.lock();
        }
        if (targetUnit.isValid() && operator==(bL->getOuterUnit(), targetUnit)) {
          bL->build();
        } else {
          Debug("Agh!  I'm lost and confused");
        }
      }
      break;
    default:
      break;
  }
}

void Cannon::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  /*cout << "Firing at " << server::world->getTimeNow() << endl;*/
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  server::world->addBallistic(new Shell(firer, status));
}

Shell::Shell(const Ref<LayeredUnit>& source, const WeaponStatus& status) :
  Ballistic(
      source,
      server::world->getTimeNow(),
      source->getIStatus()->getPosition() /* start position */,
      status.getTargetDirection() /* start velocity */
    )
{
}

void Shell::onCollision(const Point<sint32>& pos)
{
  cout << "Bang!! " << pos << endl;
  server::world->addEffect(new Explosion(getOwner(), pos, 2000));
}

void Explosion::onUnitPresent(const Ref<LayeredUnit>& victim)
{
  cout << "Explosion engulfed someone" << endl;
  victim->damage(100);
}

bool Paralyzer::aim(
    const Ref<LayeredUnit>& firer,
    WeaponStatus* status,
    const WeaponOrders& orders
  )
{
  Point<sint32> displacement =
    orders.getTargetPosition() - firer->getStatus()->getPosition() +
    Point<sint32>(0, 0, 0);
  /* test whether target is within range */
  if (displacement.squareLength() < 100000000) {
    status->setTargetDirection(displacement);
    return true;
  } else {
    return false;
  }
}

void Paralyzer::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  cout << "Firing paralyzer at time " << server::world->getTimeNow() <<
    ", in direction " << status.getTargetDirection() << endl;
  server::world->addBeam(new ParalyzationBeam(firer, status));
}

ParalyzationBeam::ParalyzationBeam(
    const Ref<LayeredUnit>& source,
    const WeaponStatus& status
  ) :
  Beam(
      source->getStatus()->getPosition(),
      status.getTargetDirection(), source,
      server::world->getTimeNow(), 3 /* duration */
    )
{
}

void ParalyzationBeam::onInteractLand(double position)
{
  cout << "ParalyzationBeam::onInteractLand at " << evaluate(position) << endl;
}

void ParalyzationBeam::onInteractUnit(double position, const Ref<LayeredUnit>&)
{
  cout << "ParalyzationBeam::onInteractUnit at " << evaluate(position) << endl;
}

extern "C" {

Weapon* spawn_cannon(const WeaponType* type)
{
  return new Cannon(type);
}

Weapon* spawn_paralyzer(const WeaponType* type)
{
  return new Paralyzer(type);
}

Weapon* spawn_factorycreater(const WeaponType* type)
{
  return new FactoryCreater(type);
}

Weapon* spawn_gruntcreater(const WeaponType* type)
{
  return new GruntCreater(type);
}

Weapon* spawn_builder(const WeaponType* type)
{
  return new Builder(type);
}

}

