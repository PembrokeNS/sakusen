#include "settingstree/pluginbranch.h"

#include "settingstree/stringleaf.h"
#include "settingstree/stringsetleaf.h"
#include "settingstree/boolleaf.h"

using namespace std;

using namespace fuseki::settingsTree;

namespace fuseki {

PluginBranch::PluginBranch(
    const String& name,
    Plugin* p,
    Branch* parent,
    Server* server
  ) :
  Branch(name, "world", "", parent, server),
  plugin(p)
{
}

}

