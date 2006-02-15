#ifndef UNITSTATUS_METHODS_H
#define UNITSTATUS_METHODS_H

#include "unitstatus.h"

#include "world.h"

namespace sakusen {

inline const UnitType* UnitStatus::getTypePtr(void) const {
  return world->getUniverse()->getUnitTypePtr(type);
}

}

#endif // UNITSTATUS_METHODS_H

