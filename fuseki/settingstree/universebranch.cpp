#include "universebranch.h"

#include "settingstree/stringleaf.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

UniverseBranch::UniverseBranch(Branch* parent, Server* server) :
  Branch("universe", "world", "", parent, server)
{
  addChild(Node::Ptr(new StringLeaf("name", "world", "admin", this, server)));
  addChild(Node::Ptr(new StringLeaf("hash", "world", "", this, server)));
}

