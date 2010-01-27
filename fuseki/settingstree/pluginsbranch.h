#ifndef FUSEKI__SETTINGSTREE__PLUGINSBRANCH_H
#define FUSEKI__SETTINGSTREE__PLUGINSBRANCH_H

#include "settingstree/pluginbranch.h"

namespace fuseki {
namespace settingsTree {

class PluginsBranch : public Branch {
  public:
    typedef boost::shared_ptr<PluginsBranch> Ptr;

    PluginsBranch(Branch* parent, Server* server);
  public:
    PluginBranch::Ptr addPlugin(const String& name, Plugin* plugin);
    void removePlugin(const String& name);
};

}}

#endif // FUSEKI__SETTINGSTREE__PLUGINSBRANCH_H

