#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"

namespace testload {

class Cannon : public sakusen::server::Weapon {
  private:
    Cannon();
  public:
    Cannon(const sakusen::WeaponType* type) :
      Weapon(type)
    {}

    Weapon* newCopy() const {
      return new Cannon(*this);
    }
};

}

extern "C" {

sakusen::server::Weapon* spawn_cannon(const sakusen::WeaponType* type);

}

#endif // CANNON_H

