#ifndef OARCHIVE_METHODS_H
#define OARCHIVE_METHODS_H

#include "oarchive.h"

#include "weapontype.h"
#include "unittype.h"

namespace sakusen {

template<>
inline void OArchive::store<uint8>(const uint8& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<sint8>(const sint8& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<uint16>(const uint16& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<sint16>(const sint16& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<uint32>(const uint32& toStore)
{
  *this << toStore;
}

template<>
inline void OArchive::store<sint32>(const sint32& toStore)
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

template<typename T>
inline void OArchive::store(const T& toStore)
{
  toStore.store(*this);
}

}

#endif // OARCHIVE_METHODS_H

