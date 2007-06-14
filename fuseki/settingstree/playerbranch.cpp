#include "settingstree/playerbranch.h"

#include "settingstree/stringleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace sakusen;
using namespace sakusen::comms;
using namespace fuseki;
using namespace fuseki::settingsTree;

PlayerBranch::PlayerBranch(
    PlayerId id,
    Branch* parent,
    Server* server,
    const PlayerTemplate& t
  ) :
  Branch(playerId_toString(id), "world", "", parent, server)
{
  String playerGroup = String("player") + playerId_toString(id);
  String playerAdminGroup = playerGroup+",admin";
  
  addChild(Node::Ptr(new StringLeaf(
          "race", "world", ( t.isRaceFixed() ? "" : playerAdminGroup ), this,
          server
        )));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "maxclients", "world", playerAdminGroup, this, server, 255
        )));
  addChild(Node::Ptr(new BoolLeaf(
          "noclients", "world", "", this, server, t.isNoClients()
        )));
  addChild(Node::Ptr(new BoolLeaf(
          "racefixed", "world", "", this, server, t.isRaceFixed()
        )));
}
  
