#include "settingstree/playersbranch.h"

#include "settingstree/playerbranch.h"

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

PlayersBranch::PlayersBranch(const Branch* parent, Server* server) :
  Branch("players", "world", "", parent, server)
{
}

PlayersBranch::~PlayersBranch()
{
}

void PlayersBranch::addPlayer(PlayerID id, const PlayerTemplate& t)
{
  String name = playerID_toString(id);
  if (getChild(name) != NULL) {
    Fatal("tried to add player branch of existing name");
  }
  addChild(new PlayerBranch(id, this, server, t));
}

void PlayersBranch::removePlayer(PlayerID id)
{
  String name = playerID_toString(id);
  if (getChild(name) == NULL) {
    Fatal("tried to remove non-existant player branch " << name);
  }
  removeChild(name);
}

