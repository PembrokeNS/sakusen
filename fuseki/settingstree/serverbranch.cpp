#include "settingstree/serverbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/stringlistleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

ServerBranch::ServerBranch(Branch* parent, Server* server) :
  Branch("server", "world", "", parent, server)
{
  addChild(Node::Ptr(new ApplicationBranch("world", "", "", this, server)));
  addChild(Node::Ptr(
        new StringListLeaf("blockedaddresses", "world", "admin", this, server)
      ));
  addChild(Node::Ptr(
        new IntLeaf<uint8>("countdown", 5, "world", "admin", this, server)
      ));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "deadclienttimeout", 5, "world", "admin", this, server
        )));
  addChild(Node::Ptr(
        new BoolLeaf("allowobservers", false, "world", "admin", this, server)
      ));
}

