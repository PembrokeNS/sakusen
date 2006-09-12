#include "cannon.h"
#include "completeworld.h"

#include <iostream>

using namespace std;
using namespace sakusen;
using namespace sakusen::server;
using namespace testsrc;

void Cannon::onFire(LayeredUnit* firer, uint16 weaponIndex)
{
  /*cout << "Firing at " << server::world->getTimeNow() << endl;*/
  const WeaponStatus& status =
    firer->getStatus()->getWeaponsStatus()[weaponIndex];
  server::world->addBallistic(new Shell(firer, status));
}

Shell::Shell(LayeredUnit* source, const WeaponStatus& status) :
  Ballistic(
      server::world->getTimeNow(),
      source->getIStatus()->getPosition() /* start position */,
      status.getDirection() /* start velocity */
    )
{
}

void Shell::onCollision(const Point<sint32>& pos)
{
  cout << "Bang!! " << pos << endl;
}

extern "C" {

Weapon* spawn_cannon(const WeaponType* type)
{
  return new Cannon(type);
}

}

