#include "cannon.h"
#include "completeworld.h"

#include <iostream>

using namespace std;
using namespace testsrc;

void Cannon::onFire(LayeredUnit* firer, uint16 /*weaponIndex*/)
{
  server::world->addBallistic(new Shell(firer));
}

Shell::Shell(LayeredUnit* source) :
  Ballistic(
      server::world->getTimeNow(),
      source->getIStatus()->getPosition() /* start position */,
      Point<sint32>() /* start velocity */
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

