#include "region-methods.h"

namespace sakusen {

template<>
inline bool Region<sint32>::contains(const IUnitStatus* unit) const
{
  /* TODO: maybe we want to check whether it intersects the unit or completely
   * contains the unit, rather than simply whether it contains the centre, as
   * here */
  return contains(unit->getPosition());
}

}
