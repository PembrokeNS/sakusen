#include "settingstree/serverbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/stringsetleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

ServerBranch::ServerBranch(Branch* parent, Server* server) :
  Branch("server", "world", "", parent, server)
{
  addChild(Node::Ptr(new ApplicationBranch("world", "", "", this, server)));
  addChild(Node::Ptr(
        new StringSetLeaf("blockedaddresses", "world", "admin", this, server)
      ));
  addChild(Node::Ptr(
        new StringSetLeaf("plugins", "world", "admin", this, server)
      ));
  addChild(Node::Ptr(
        new IntLeaf<uint8>("countdown", "world", "admin", this, server, 5)
      ));
  addChild(Node::Ptr(new IntLeaf<uint8>(
          "deadclienttimeout", "world", "admin", this, server, 5
        )));
  addChild(Node::Ptr(
        new BoolLeaf("allowobservers", "world", "admin", this, server, false)
      ));
}

