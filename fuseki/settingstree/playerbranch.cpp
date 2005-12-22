#include "settingstree/playerbranch.h"

#include "settingstree/stringleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace fuseki::settingsTree;

PlayerBranch::PlayerBranch(
    PlayerID id,
    const Branch* parent,
    Server* server,
    bool special,
    bool fixed
  ) :
  Branch(playerID_toString(id), "world", "", parent, server)
{
  assert(fixed || !special);
  String playerGroup = String("player") + clientID_toString(id);
  String playerAdminGroup = playerGroup+",admin";
  
  addChild(new StringLeaf(
        "race", "world", ( fixed ? "" : playerAdminGroup ), this, server
      ));
  addChild(new IntLeaf<uint8>(
        "maxclients", 255, "world", playerAdminGroup, this, server
      ));
  addChild(new BoolLeaf("special", special, "world", "", this, server));
  addChild(new BoolLeaf("fixed", fixed, "world", "", this, server));
}
  
