#ifndef LIBSAKUSEN__UNITSTATUS_METHODS_H
#define LIBSAKUSEN__UNITSTATUS_METHODS_H

#include "unitstatus.h"

#include "world.h"

namespace sakusen {

inline const UnitType* UnitStatus::getTypePtr(void) const {
  return world->getUniverse()->getUnitTypePtr(type);
}

}

#endif // LIBSAKUSEN__UNITSTATUS_METHODS_H

