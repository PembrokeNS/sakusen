#include "settingstree/applicationbranch.h"

#include "settingstree/stringleaf.h"

using namespace fuseki;
using namespace fuseki::settingsTree;

ApplicationBranch::ApplicationBranch(
    const String& readers,
    const String& writers,
    const String& subWriters,
    Branch* parent,
    Server* server
  ) :
  Branch("application", readers, writers, parent, server)
{
  addChild(
      Node::Ptr(new StringLeaf("name", readers, subWriters, this, server))
    );
  addChild(
      Node::Ptr(new StringLeaf("version", readers, subWriters, this, server))
    );
  addChild(
      Node::Ptr(new StringLeaf("revision", readers, subWriters, this, server))
    );
}

