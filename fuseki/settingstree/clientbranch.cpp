#include "settingstree/clientbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/boolleaf.h"
#include "settingstree/intleaf.h"

using namespace sakusen::comms;
using namespace fuseki;
using namespace fuseki::settingsTree;

ClientBranch::ClientBranch(ClientID id, Branch* parent, Server* server) :
  Branch(clientID_toString(id), "world", "", parent, server)
{
  String clientGroup = String("client") + clientID_toString(id);
  addChild(Node::Ptr(
        new ApplicationBranch("world", "", clientGroup, this, server)
      ));
  addChild(Node::Ptr(
        new BoolLeaf("admin", false, "world", "admin", this, server)
      ));
  addChild(Node::Ptr(
        new BoolLeaf("neveradmin", true, "world", clientGroup, this, server)
      ));
  addChild(Node::Ptr(new BoolLeaf(
          "observer", false, "world", clientGroup+",admin", this, server
        )));
  addChild(Node::Ptr(
        new BoolLeaf("ready", false, "world", clientGroup, this, server)
      ));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "player", 0, "world", clientGroup+",admin", this, server
        )));
}

