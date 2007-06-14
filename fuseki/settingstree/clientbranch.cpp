#include "settingstree/clientbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/boolleaf.h"
#include "settingstree/intleaf-methods.h"

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

ClientBranch::ClientBranch(ClientId id, Branch* parent, Server* server) :
  Branch(clientId_toString(id), "world", "", parent, server)
{
  String clientGroup = String("client") + clientId_toString(id);
  addChild(Node::Ptr(
        new ApplicationBranch("world", "", clientGroup, this, server)
      ));
  addChild(Node::Ptr(
        new BoolLeaf("admin", "world", "admin", this, server, false)
      ));
  addChild(Node::Ptr(
        new BoolLeaf("neveradmin", "world", clientGroup, this, server, true)
      ));
  addChild(Node::Ptr(new BoolLeaf(
          "observer", "world", clientGroup+",admin", this, server, false
        )));
  addChild(Node::Ptr(
        new BoolLeaf("ready", "world", clientGroup, this, server, false)
      ));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "player", "world", clientGroup+",admin", this, server, 0
        )));
}

