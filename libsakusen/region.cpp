#include "region-methods.h"

namespace sakusen {

template<>
inline bool Region<sint32>::contains(const IUnitStatus* unit) const
{
  /** \todo: maybe we want to check whether it intersects the unit or completely
   * contains the unit, rather than simply whether it contains the centre, as
   * here */
  return contains(unit->getPosition());
}

template<>
inline bool Region<sint16>::contains(const IUnitStatus* unit) const
{
  return contains(unit->getPosition());
}


#ifdef _MSC_VER
template LIBSAKUSEN_API Region<sint16>;
template LIBSAKUSEN_API Region<sint32>;
#endif

}
