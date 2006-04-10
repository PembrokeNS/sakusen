#include "settingstree/gamebranch.h"

#include "fuseki-global.h"
#include "settingstree/stringleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"
#include "settingstree/universebranch.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

GameBranch::GameBranch(const Branch* parent, Server* server) :
  Branch("game", "world", "", parent, server)
{
  addChild(new UniverseBranch(this, server));
  addChild(new StringLeaf("map", "world", "admin", this, server));
  addChild(
      new IntLeaf<uint8>("mapplaymode", 0, "world", "admin", this, server)
    );
  addChild(new IntLeaf<uint32>(
        "speed", DEFAULT_GAME_SPEED, "world", "admin,playtime", this, server
      ));
  addChild(
      new BoolLeaf("paused", false, "world", "admin,playtime", this, server)
    );
}

