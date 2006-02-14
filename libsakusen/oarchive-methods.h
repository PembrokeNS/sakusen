#ifndef OARCHIVE_METHODS_H
#define OARCHIVE_METHODS_H

#include "oarchive.h"

#include "weapontype.h"
#include "unit.h"

namespace sakusen {

template<>
inline void OArchive::store<uint32>(const uint32& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<String>(const String& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<WeaponTypeID>(const WeaponTypeID& toStore)
{
  *this << toStore->getInternalName();
}

template<>
inline void OArchive::store<UnitTypeID>(const UnitTypeID& toStore)
{
  *this << toStore->getInternalName();
}

template<>
inline void OArchive::store<Unit*>(Unit* const& toStore)
{
  *this << toStore->getId();
}

}

#endif // OARCHIVE_METHODS_H

