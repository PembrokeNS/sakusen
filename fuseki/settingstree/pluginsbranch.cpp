#include "settingstree/pluginsbranch.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

PluginsBranch::PluginsBranch(Branch* parent, Server* server) :
  Branch("plugins", "world", "", parent, server)
{
}

PluginBranch::Ptr PluginsBranch::addPlugin(const String& name, Plugin* plugin)
{
  if (getChild(name)) {
    SAKUSEN_FATAL("tried to add plugin branch of existing name " << name);
  }
  PluginBranch::Ptr newBranch(new PluginBranch(name, plugin, this, server));
  addChild(newBranch);
  return newBranch;
}

void PluginsBranch::removePlugin(const String& name)
{
  if (!getChild(name)) {
    SAKUSEN_FATAL("tried to remove non-existant plugin branch " << name);
  }
  removeChild(name);
}

