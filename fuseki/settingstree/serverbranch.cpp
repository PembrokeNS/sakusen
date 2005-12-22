#include "settingstree/serverbranch.h"

#include "settingstree/applicationbranch.h"
#include "settingstree/stringlistleaf.h"
#include "settingstree/intleaf-methods.h"
#include "settingstree/boolleaf.h"

using namespace fuseki::settingsTree;

ServerBranch::ServerBranch(const Branch* parent, Server* server) :
  Branch("server", "world", "", parent, server)
{
  addChild(new ApplicationBranch("world", "", "", this, server));
  addChild(
      new StringListLeaf("blockedaddresses", "world", "admin", this, server)
    );
  addChild(new IntLeaf<uint8>("countdown", 5, "world", "admin", this, server));
  addChild(new IntLeaf<uint8>(
        "deadclienttimeout", 5, "world", "admin", this, server
      ));
  addChild(
      new BoolLeaf("allowobservers", false, "world", "admin", this, server)
    );
}

ServerBranch::~ServerBranch()
{
}

