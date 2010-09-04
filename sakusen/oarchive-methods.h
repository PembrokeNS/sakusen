#ifndef LIBSAKUSEN__OARCHIVE_METHODS_H
#define LIBSAKUSEN__OARCHIVE_METHODS_H

#include <sakusen/oarchive.h>

#include <sakusen/weapontype.h>
#include <sakusen/unittype.h>

namespace sakusen {

#define SAKUSEN_OARCHIVE_SIMPLE_STORER(type) \
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

SAKUSEN_OARCHIVE_SIMPLE_STORER(uint8)
SAKUSEN_OARCHIVE_SIMPLE_STORER(sint8)
SAKUSEN_OARCHIVE_SIMPLE_STORER(uint16)
SAKUSEN_OARCHIVE_SIMPLE_STORER(sint16)
SAKUSEN_OARCHIVE_SIMPLE_STORER(uint32)
SAKUSEN_OARCHIVE_SIMPLE_STORER(sint32)
SAKUSEN_OARCHIVE_SIMPLE_STORER(String)

#undef SAKUSEN_OARCHIVE_SIMPLE_STORER

template<>
struct Storer<WeaponTypeId> {
  void operator()(OArchive& archive, const WeaponTypeId toStore)
  {
    archive << toStore->getInternalName();
  }
};

template<>
struct Storer<UnitTypeId> {
  void operator()(OArchive& archive, const UnitTypeId toStore)
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

#endif // LIBSAKUSEN__OARCHIVE_METHODS_H

