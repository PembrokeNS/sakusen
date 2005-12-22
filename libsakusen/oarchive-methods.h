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

template<
    typename T,
    template<typename> class Container1,
    template<typename> class Container2
  >
OArchive& OArchive::operator<<(const Container1< Container2<T> >& toStore)
{
  *this << uint32(toStore.size());
  
  for (typename Container1< Container2<T> >::const_iterator
      i1 = toStore.begin(); i1 != toStore.end(); ++i1) {
    *this << uint32(i1->size());
    for (typename Container2<T>::const_iterator
        i2 = i1->begin(); i2 != i1->end(); ++i2) {
      store(*i2);
    }
  }

  return *this;
}

template<typename T, template<typename> class Container>
OArchive& OArchive::operator<<(const Container<T>& toStore)
{
  *this << uint32(toStore.size());
  
  for(typename Container<T>::const_iterator i = toStore.begin();
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

