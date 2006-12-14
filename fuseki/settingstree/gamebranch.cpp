#include "settingstree/gamebranch.h"

#include "fuseki-global.h"
#include "settingstree/stringleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"
#include "settingstree/universebranch.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

GameBranch::GameBranch(Branch* parent, Server* server) :
  Branch("game", "world", "", parent, server)
{
  addChild(Node::Ptr(new UniverseBranch(this, server)));
  addChild(Node::Ptr(new StringLeaf("map", "world", "admin", this, server)));
  addChild(Node::Ptr(
        new IntLeaf<uint8>("mapplaymode", 0, "world", "admin", this, server)
      ));
  addChild(Node::Ptr(new IntLeaf<uint32>(
          "speed", DEFAULT_GAME_SPEED, "world", "admin,playtime", this, server
        )));
  addChild(Node::Ptr(
        new BoolLeaf("paused", false, "world", "admin,playtime", this, server)
      ));
}

