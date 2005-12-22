#ifndef REMOTECLIENT_METHODS_H
#define REMOTECLIENT_METHODS_H

#include "remoteclient.h"
#include "server.h"

namespace fuseki {

template<typename T>

String RemoteClient::performIntMagic(
    /*const settingsTree::Leaf* altering,*/
    const std::list<String>& name,
    T /*newValue*/
  )
{
  assert(!name.empty());
  if (name.front() == "observer") {
    Fatal("observer not an int setting");
  } else if (name.front() == "application") {
    /* Do nothing */
  } else {
    Fatal("unexpected child of client branch: " << name.front());
  }
  return "";
}

}

#endif // REMOTECLIENT_METHODS_H

