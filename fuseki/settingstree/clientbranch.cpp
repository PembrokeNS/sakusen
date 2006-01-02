#include "settingstree/clientbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/boolleaf.h"
#include "settingstree/intleaf.h"

using namespace fuseki::settingsTree;

ClientBranch::ClientBranch(ClientID id, const Branch* parent, Server* server) :
  Branch(clientID_toString(id), "world", "", parent, server)
{
  String clientGroup = String("client") + clientID_toString(id);
  addChild(new ApplicationBranch("world", "", clientGroup, this, server));
  addChild(new BoolLeaf("admin", false, "world", "admin", this, server));
  addChild(
      new BoolLeaf("neveradmin", true, "world", clientGroup, this, server)
    );
  addChild(new BoolLeaf(
        "observer", false, "world", clientGroup+",admin", this, server
      ));
  addChild(new BoolLeaf("ready", false, "world", clientGroup, this, server));
  addChild(new IntLeaf<uint8>(
        "player", 0, "world", clientGroup+",admin", this, server
      ));
}

