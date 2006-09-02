#include "cannon.h"

using namespace sakusen;
using namespace sakusen::server;
using namespace testload;

extern "C" {

Weapon* spawn_cannon(const WeaponType* type)
{
  return new Cannon(type);
}

}

