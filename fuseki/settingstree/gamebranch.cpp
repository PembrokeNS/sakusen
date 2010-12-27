#include "settingstree/gamebranch.h"

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
        new IntLeaf<uint8>("mapplaymode", "world", "admin", this, server)
      ));
  addChild(Node::Ptr(new IntLeaf<uint32>(
          "speed", "world", "admin,playtime", this, server,
          FUSEKI_DEFAULT_GAME_SPEED
        )));
  addChild(Node::Ptr(
        new BoolLeaf("paused", "world", "admin,playtime", this, server, false)
      ));
}

