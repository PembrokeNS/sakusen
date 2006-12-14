#include "settingstree/playerbranch.h"

#include "settingstree/stringleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace sakusen;
using namespace sakusen::comms;
using namespace fuseki;
using namespace fuseki::settingsTree;

PlayerBranch::PlayerBranch(
    PlayerID id,
    Branch* parent,
    Server* server,
    const PlayerTemplate& t
  ) :
  Branch(playerID_toString(id), "world", "", parent, server)
{
  String playerGroup = String("player") + playerID_toString(id);
  String playerAdminGroup = playerGroup+",admin";
  
  addChild(Node::Ptr(new StringLeaf(
          "race", "world", ( t.isRaceFixed() ? "" : playerAdminGroup ), this,
          server
        )));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "maxclients", 255, "world", playerAdminGroup, this, server
        )));
  addChild(Node::Ptr(new BoolLeaf(
          "noclients", t.isNoClients(), "world", "", this, server
        )));
  addChild(Node::Ptr(new BoolLeaf(
          "racefixed", t.isRaceFixed(), "world", "", this, server
        )));
}
  
