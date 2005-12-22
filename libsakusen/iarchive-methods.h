#ifndef IARCHIVE_METHODS_H
#define IARCHIVE_METHODS_H

#include "iarchive.h"
#include "unit.h"

template<>
inline String IArchive::load<String>()
{
  String s;
  *this >> s;
  return s;
}

template<typename T>
inline T IArchive::load()
{
  return T::load(*this);
}

template<typename T>
inline T IArchive::load(const typename T::loadArgument* arg)
{
  return T::load(*this, arg);
}

template<typename T, template<typename> class Container>
IArchive& IArchive::operator>>(Container<T>& result)
{
  assert(result.empty());
  uint32 size;
  *this >> size;
  
  while (size--) {
    result.push_back(load<T>());
  }

  return *this;
}

template<typename T, template<typename> class Container>
IArchive& IArchive::extract(
    Container<T>& result,
    const typename T::loadArgument* arg
  )
{
  assert(result.empty());
  uint32 size;
  *this >> size;
  
  while (size--) {
    result.push_back(load<T>(arg));
  }

  return *this;
}

template<
    typename T,
    template<typename> class Container1,
    template<typename> class Container2
  >
IArchive& IArchive::extract(
    Container1< Container2<T> >& result,
    const typename T::loadArgument* arg
  )
{
  assert(result.empty());
  uint32 size;
  *this >> size;
  
  while (size--) {
    Container2<T> element;
    extract(element, arg);
    result.push_back(element);
  }

  return *this;
}

template<typename T>
IArchive& IArchive::operator>>(Point<T>& result)
{
  T x;
  *this >> x;
  T y;
  *this >> y;
  T z;
  *this >> z;

  result = Point<T>(x,y,z);
  return *this;
}

#endif // IARCHIVE_METHODS_H

