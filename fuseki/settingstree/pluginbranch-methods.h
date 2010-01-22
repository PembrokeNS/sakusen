#ifndef FUSEKI__SETTINGSTREE__PLUGINBRANCH_METHODS_H
#define FUSEKI__SETTINGSTREE__PLUGINBRANCH_METHODS_H

#include "settingstree/pluginbranch.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/treetraits.h"

namespace fuseki {
namespace settingsTree {

template<typename T>
void PluginBranch::addLeaf(
    const String& name,
    typename boost::call_traits<T>::param_type defaultVal
  ) {
  if (getChild(name)) {
    SAKUSEN_FATAL("tried to add already existing child " << name <<
        " to plugin branch " << getName());
  }
  addChild(Node::Ptr(new typename TreeTraits<T>::LeafType(
          name, "world", "admin", this, server, defaultVal
        )));
}

}}

#endif // FUSEKI__SETTINGSTREE__PLUGINBRANCH_METHODS_H

