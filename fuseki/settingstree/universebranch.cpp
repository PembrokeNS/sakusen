#include "universebranch.h"

#include "settingstree/stringleaf.h"

using namespace fuseki::settingsTree;

UniverseBranch::UniverseBranch(const Branch* parent, Server* server) :
  Branch("universe", "world", "", parent, server)
{
  addChild(new StringLeaf("name", "world", "admin", this, server));
  addChild(new StringLeaf("hash", "world", "", this, server));
}

