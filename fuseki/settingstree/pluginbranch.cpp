#include "settingstree/pluginbranch.h"

#include "settingstree/stringleaf.h"
#include "settingstree/stringsetleaf.h"
#include "settingstree/boolleaf.h"

using namespace std;

using namespace fuseki::settingsTree;

PluginBranch::PluginBranch(
    const String& name,
	fuseki::Plugin* p,
	Branch* parent,
	fuseki::Server* server
  ) :
  Branch(name, "world", "", parent, server),
  plugin(p)
{
}

