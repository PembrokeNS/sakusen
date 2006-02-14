#ifndef INTLEAF_METHODS_H
#define INTLEAF_METHODS_H

#include "intleaf.h"

#include "server-methods.h"

namespace fuseki {
namespace settingsTree {

template<typename T>
IntLeaf<T>::IntLeaf(
    const String& name,
    T v,
    const String& readers,
    const String& writers,
    const Branch* parent,
    Server* server
  ) :
  Leaf(name, readers, writers, parent, server),
  value(v)
{
}  

template<typename T>
String IntLeaf<T>::setValue(const String& s)
{
  T v = sakusen::numFromString<T>(s);
  if (v == value) {
    return "setting already has that value";
  }
  String reason;
  if ("" != (reason = server->intSettingAlteringCallback(this, v))) {
    return reason;
  }

  value = v;
  return "";
}

template<typename T>
String IntLeaf<T>::getValue() const
{
  return sakusen::numToString(value);
}

}}

#endif // INTLEAF_METHODS_H

