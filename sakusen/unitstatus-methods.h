#ifndef LIBSAKUSEN__UNITSTATUS_METHODS_H
#define LIBSAKUSEN__UNITSTATUS_METHODS_H

#include <sakusen/unitstatus.h>

#include <sakusen/world.h>

namespace sakusen {

inline const UnitType* UnitStatus::getTypePtr(void) const {
  return world->getUniverse()->getUnitTypePtr(type);
}

}

#endif // LIBSAKUSEN__UNITSTATUS_METHODS_H

