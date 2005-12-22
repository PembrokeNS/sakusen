#include "settingstree/playersbranch.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

PlayersBranch::PlayersBranch(const Branch* parent, Server* server) :
  Branch("players", "world", "", parent, server)
{
}

PlayersBranch::~PlayersBranch()
{
}

