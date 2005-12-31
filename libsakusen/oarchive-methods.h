#ifndef OARCHIVE_METHODS_H
#define OARCHIVE_METHODS_H

#include "oarchive.h"
#include "weapontype.h"

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

template<typename T>
inline void OArchive::store(const T& toStore)
{
  toStore.store(*this);
}

template<typename T>
OArchive& OArchive::operator<<(const std::vector< std::vector<T> >& toStore)
{
  *this << uint32(toStore.size());
  
  for (typename std::vector< std::vector<T> >::const_iterator
      i1 = toStore.begin(); i1 != toStore.end(); ++i1) {
    *this << uint32(i1->size());
    for (typename std::vector<T>::const_iterator
        i2 = i1->begin(); i2 != i1->end(); ++i2) {
      store(*i2);
    }
  }

  return *this;
}

template<typename T>
OArchive& OArchive::operator<<(const std::vector<T>& toStore)
{
  *this << uint32(toStore.size());
  
  for(typename std::vector<T>::const_iterator i = toStore.begin();
      i != toStore.end(); ++i) {
    store(*i);
  }

  return *this;
}

template<typename T>
OArchive& OArchive::operator<<(const std::list<T>& toStore)
{
  *this << uint32(toStore.size());
  
  for(typename std::list<T>::const_iterator i = toStore.begin();
      i != toStore.end(); ++i) {
    store(*i);
  }

  return *this;
}

template<typename T>
inline OArchive& OArchive::operator<<(const Point<T>& p)
{
  return *this << p.x << p.y << p.z;
}

#endif // OARCHIVE_METHODS_H

