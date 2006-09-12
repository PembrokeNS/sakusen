#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"
#include "ballistic.h"
#include "layeredunit.h"

using namespace sakusen;
using namespace sakusen::server;

namespace testsrc {

class Cannon : public Weapon {
  private:
    Cannon();
  public:
    Cannon(const WeaponType* type) :
      Weapon(type)
    {}
  protected:
    uint32 getProjectileSpeed() { return 20; }
  public:
    void onFire(LayeredUnit* firer, uint16 weaponIndex);

    Weapon* newCopy() const {
      return new Cannon(*this);
    }
};

class Shell : public Ballistic {
  private:
    Shell();
  public:
    Shell(LayeredUnit* source, const WeaponStatus& status);

    void onCollision(const Point<sint32>& pos);
};

}

extern "C" {

Weapon* spawn_cannon(const WeaponType* type);

}

#endif // CANNON_H

