#ifndef FUSEKI__SETTINGSTREE__PLUGINBRANCH_H
#define FUSEKI__SETTINGSTREE__PLUGINBRANCH_H

#include <boost/call_traits.hpp>

#include "settingstree/branch.h"

namespace fuseki {

class Plugin;

namespace settingsTree {

class PluginBranch : public Branch {
  public:
    typedef boost::shared_ptr<PluginBranch> Ptr;
    typedef boost::shared_ptr<const PluginBranch> ConstPtr;

    PluginBranch(
        const String& name,
        Plugin* plugin,
        Branch* parent,
        Server* server
    );
  private:
    Plugin* plugin;
  public:
    template<typename T>
    void addLeaf(
        const String& name,
        typename boost::call_traits<T>::param_type defaultVal
    );
};

}}

#endif // FUSEKI__SETTINGSTREE__PLUGINBRANCH_H

