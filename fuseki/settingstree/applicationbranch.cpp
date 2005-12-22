#include "settingstree/applicationbranch.h"

#include "settingstree/stringleaf.h"

using namespace fuseki::settingsTree;

ApplicationBranch::ApplicationBranch(
    const String& readers,
    const String& writers,
    const String& subWriters,
    const Branch* parent,
    Server* server
  ) :
  Branch("application", readers, writers, parent, server)
{
  addChild(new StringLeaf("name", readers, subWriters, this, server));
  addChild(new StringLeaf("version", readers, subWriters, this, server));
  addChild(new StringLeaf("id", readers, subWriters, this, server));
}

ApplicationBranch::~ApplicationBranch()
{
}

