#include "settingstree/playersbranch.h"

#include "settingstree/playerbranch.h"

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

PlayersBranch::PlayersBranch(Branch* parent, Server* server) :
  Branch("players", "world", "", parent, server)
{
}

void PlayersBranch::addPlayer(PlayerId id, const PlayerTemplate& t)
{
  String name = playerId_toString(id);
  if (getChild(name) != NULL) {
    Fatal("tried to add player branch of existing name");
  }
  addChild(Node::Ptr(new PlayerBranch(id, this, server, t)));
}

void PlayersBranch::removePlayer(PlayerId id)
{
  String name = playerId_toString(id);
  if (getChild(name) == NULL) {
    Fatal("tried to remove non-existant player branch " << name);
  }
  removeChild(name);
}

