#ifndef OARCHIVE_METHODS_H
#define OARCHIVE_METHODS_H

#include "oarchive.h"

#include "weapontype.h"
#include "unittype.h"

namespace sakusen {

#define SIMPLE_STORER(type) \
template<>                  \
struct Storer<type> {       \
  void operator()(          \
      OArchive& archive,    \
      boost::call_traits<type>::param_type toStore \
    )                       \
  {                         \
    archive << toStore;     \
  }                         \
};

SIMPLE_STORER(uint8)
SIMPLE_STORER(sint8)
SIMPLE_STORER(uint16)
SIMPLE_STORER(sint16)
SIMPLE_STORER(uint32)
SIMPLE_STORER(sint32)
SIMPLE_STORER(String)

#undef SIMPLE_STORER

template<>
struct Storer<WeaponTypeID> {
  void operator()(OArchive& archive, const WeaponTypeID toStore)
  {
    archive << toStore->getInternalName();
  }
};

template<>
struct Storer<UnitTypeID> {
  void operator()(OArchive& archive, const UnitTypeID toStore)
  {
    archive << toStore->getInternalName();
  }
};

template<typename T, typename U>
struct Storer<std::pair<T,U> > {
  void operator()(OArchive& archive, const std::pair<T,U>& toStore)
  {
    archive << toStore;
  }
};

template<typename T>
struct Storer<Point<T> > {
  void operator()(OArchive& archive, const Point<T>& toStore)
  {
    archive << toStore;
  }
};

template<typename T>
struct Storer<std::vector<T> > {
  void operator()(OArchive& archive, const std::vector<T>& toStore)
  {
    archive << toStore;
  }
};

template<typename T>
inline void Storer<T>::operator()(
    OArchive& archive,
    typename boost::call_traits<T>::param_type toStore
  )
{
  toStore.store(archive);
}

}

#endif // OARCHIVE_METHODS_H

