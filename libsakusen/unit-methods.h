#ifndef UNIT_METHODS_H
#define UNIT_METHODS_H

#include "unit.h"

#include "world.h"

namespace sakusen {

inline const UnitType* Unit::getTypePtr(void) const {
  return world->getUniverse()->getUnitTypePtr(type);
}

}

#endif // UNIT_METHODS_H

