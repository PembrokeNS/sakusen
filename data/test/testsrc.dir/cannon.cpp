#include "cannon.h"
#include <sakusen/server/completeworld.h>

#include <iostream>

using namespace std;
using namespace sakusen;
using namespace sakusen::server;
using namespace testsrc;

void TestScript::unitAdded(const Ref<LayeredUnit>& unit) {
  const UnitStatus& status = unit->getStatus();
  const UnitType* type = status.getTypePtr();
  if (type->getInternalName() == "commander") {
    UnitMask::Ptr materielCreater(new MaterielCreater());
    unit->insertLayer(materielCreater);
  }
}

void MaterielCreater::incrementState()
{
  UnitMask::incrementState();
  Player* owner = server::world->getPlayerPtr(getOuterUnit()->getOwner());
  owner->addEnergy(energyPerTick);
  owner->addMetal(metalPerTick);
}

HitPoints BuildingLayer::build(HitPoints amount)
{
  if (builtHitPoints >= nextLayer->getMaxHitPoints()) {
    /* We're finished, remove ourselves */
    getOuterUnit()->removeLayer(this);
    return HitPoints(0);
  }

  if (builtHitPoints + amount > nextLayer->getMaxHitPoints()) {
    /* We're going to overshoot */
    amount = nextLayer->getMaxHitPoints() - builtHitPoints;
  }

  builtHitPoints += amount;
  getOuterUnit()->repair(amount, false);
  getOuterUnit()->setDirty();
  return amount;
}

bool Creater::aim(
    const Ref<LayeredUnit>& source,
    WeaponStatus& status,
    const WeaponOrders& orders
  )
{
  Position displacement =
    orders.getTargetPosition() - source->getStatus().getPosition();
  if (displacement.squareLength() <= squareRange()) {
    status.setTarget(orders.getTargetFrame());
    return true;
  }
  return false;
}

void Creater::onFire(
    const Ref<LayeredUnit>& firer,
    const WeaponStatus& status,
    WeaponOrders& orders,
    uint16 /*weaponIndex*/
  )
{
  switch (status.getTargetType()) {
    case weaponTargetType_frame:
      {
        Ref<LayeredUnit> newUnit = LayeredUnit::spawn(
            firer->getOwner(),
            getTypeCreated(),
            status.getTargetFrame(),
            Velocity(),
            HitPoints(1)
          );
        newUnit->insertLayer(BuildingLayer::Ptr(new BuildingLayer()));
        newUnit->setDirty();
        orders.clear();
      }
      break;
    default:
      break;
  }
}

bool Builder::aim(
    const Ref<LayeredUnit>& source,
    WeaponStatus& status,
    const WeaponOrders& orders
  )
{
  if (orders.getTargetType() != weaponTargetType_unit) {
    return false;
  }
  Ref<LayeredUnit> target = orders.getTargetUnit().dynamicCast<LayeredUnit>();
  Position displacement =
    target->getStatus().getPosition() - source->getStatus().getPosition();
  if (displacement.squareLength() <= squareRange()) {
    /** \todo Find and set the appropriate building layer */
    setTarget(target, status);
    return true;
  }
  return false;
}

void Builder::onFire(
    const Ref<LayeredUnit>& /*firer*/,
    const WeaponStatus& status,
    WeaponOrders& orders,
    uint16 /*weaponIndex*/
  )
{
  switch (status.getTargetType()) {
    case weaponTargetType_unit:
      {
        Ref<LayeredUnit> targetUnit = getTargetUnit();
        BuildingLayer::Ptr bL;
        if (!buildingLayer.expired()) {
          bL = buildingLayer.lock();
        }
        if (targetUnit) {
          if (!(bL && bL->getOuterUnit() == targetUnit)) {
            bL = targetUnit->getLayer<BuildingLayer>();
            if (!bL) {
              SAKUSEN_DEBUG(
                  "building target (" <<
                  targetUnit->getStatus().getType()->getInternalName() <<
                  ") not being built"
                );
              orders.clear();
              return;
            }
          }
          bL->build(HitPoints(10));
        } else {
          SAKUSEN_DEBUG("invalid building target");
        }
      }
      break;
    default:
      break;
  }
}

void Cannon::onFire(
    const Ref<LayeredUnit>& firer,
    const WeaponStatus& status,
    WeaponOrders&,
    uint16 /*weaponIndex*/
  )
{
  /*cout << "Firing at " << server::world->getTimeNow() << endl;*/
  server::world->addBallistic(new Shell(firer, status));
}

Shell::Shell(const Ref<LayeredUnit>& source, const WeaponStatus& status) :
  Ballistic(
      source,
      server::world->getTimeNow(),
      source->getIStatus().getPosition() /* start position */,
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
  victim->damage(HitPoints(10));
}

bool Paralyzer::aim(
    const Ref<LayeredUnit>& firer,
    WeaponStatus& status,
    const WeaponOrders& orders
  )
{
  Point<sint32> displacement =
    orders.getTargetPosition() - firer->getStatus().getPosition() +
    Point<sint32>(0, 0, 0);
  /* test whether target is within range */
  if (displacement.squareLength() < 100000000) {
    status.setTargetDirection(displacement);
    return true;
  } else {
    return false;
  }
}

void Paralyzer::onFire(
    const Ref<LayeredUnit>& firer,
    const WeaponStatus& status,
    WeaponOrders&,
    uint16 /*weaponIndex*/
  )
{
  cout << "Firing paralyzer at time " << server::world->getTimeNow() <<
    ", in direction " << status.getTargetDirection() << endl;
  server::world->addBeam(new ParalyzationBeam(firer, status));
}

ParalyzationBeam::ParalyzationBeam(
    const Ref<LayeredUnit>& source,
    const WeaponStatus& status
  ) :
  Beam(
      source->getStatus().getPosition(),
      status.getTargetDirection(), source,
      server::world->getTimeNow(), 3 /* duration */
    )
{
}

void ParalyzationBeam::onInteractLand(double position)
{
  cout << "ParalyzationBeam::onInteractLand at " << evaluate(position) << endl;
}

void ParalyzationBeam::onInteractUnit(
    double position,
    const Ref<LayeredUnit>& unit,
    bool leaving
  )
{
  if (leaving) {
    return;
  }
  cout << "ParalyzationBeam::onInteractUnit at " << evaluate(position) <<
    "(unitId=" << unit->getId() << ", sourceId=" << getSource()->getId() <<
    ")" << endl;
  ParalyzingLayer::Ptr layer(new ParalyzingLayer());
  unit->insertLayer(layer);
  FuseToken token =
    server::world->addFuse(layer, server::world->getTimeNow()+50);
  layer->token = token;
}

void ParalyzingLayer::expire(FuseToken) {
  Ref<LayeredUnit> u = getOuterUnit();
  if (u) {
    u->removeLayer(this);
  }
}

void ParalyzingLayer::onRemoval() {
  UnitMask::onRemoval();
  server::world->removeFuse(token);
}

extern "C" {

SAKUSEN_TESTSRC_MODULE_API Script* create_script()
{
  return new TestScript();
}

SAKUSEN_TESTSRC_MODULE_API PlayerData* create_player()
{
  return new TestPlayerData();
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_cannon(const WeaponType* type)
{
  return new Cannon(type);
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_paralyzer(const WeaponType* type)
{
  return new Paralyzer(type);
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_factorycreater(const WeaponType* type)
{
  return new FactoryCreater(type);
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_gruntcreater(const WeaponType* type)
{
  return new GruntCreater(type);
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_spidercreater(const WeaponType* type)
{
  return new SpiderCreater(type);
}

SAKUSEN_TESTSRC_MODULE_API Weapon* spawn_builder(const WeaponType* type)
{
  return new Builder(type);
}

}

