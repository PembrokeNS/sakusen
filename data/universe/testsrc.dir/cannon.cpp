#include "cannon.h"
#include "completeworld.h"

#include <iostream>

using namespace std;
using namespace sakusen;
using namespace sakusen::server;
using namespace testsrc;

void Cannon::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  /*cout << "Firing at " << server::world->getTimeNow() << endl;*/
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  server::world->addBallistic(new Shell(firer, status));
}

Shell::Shell(const Ref<LayeredUnit>& source, const WeaponStatus& status) :
  Ballistic(
      source->getOwner(),
      server::world->getTimeNow(),
      source->getIStatus()->getPosition() /* start position */,
      status.getDirection() /* start velocity */
    )
{
}

void Shell::onCollision(const Point<sint32>& pos)
{
  cout << "Bang!! " << pos << endl;
  server::world->addEffect(new Explosion(getOwner(), pos, 20));
}

void Explosion::onUnitPresent(const Ref<LayeredUnit>& victim)
{
  cout << "Explosion engulfed someone" << endl;
  victim->damage(100);
}

bool Paralyzer::aimAt(
    const Ref<LayeredUnit>& firer,
    WeaponStatus* status,
    const Point<sint32>& pos,
    const Point<sint16>& /*vel*/
  )
{
  Point<sint32> displacement = pos - firer->getStatus()->getPosition();
  /* test whether target is within range */
  if (displacement.squareLength() < 10000) {
    status->setDirection(displacement);
    return true;
  } else {
    return false;
  }
}

void Paralyzer::onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  cout << "Firing paralyzer at " << server::world->getTimeNow() << endl;
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  server::world->addBeam(new ParalyzationBeam(firer, status));
}

ParalyzationBeam::ParalyzationBeam(
    const Ref<LayeredUnit>& source,
    const WeaponStatus& status
  ) :
  Beam(
      source->getStatus()->getPosition(),
      status.getDirection(), source,
      server::world->getTimeNow(), 3 /* duration */
    )
{
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

}

