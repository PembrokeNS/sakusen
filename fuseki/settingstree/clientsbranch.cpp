#include "settingstree/clientsbranch.h"

#include "settingstree/clientbranch.h"

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

ClientsBranch::ClientsBranch(Branch* parent, Server* server) :
  Branch("clients", "world", "", parent, server)
{
}

void ClientsBranch::addClient(ClientId id)
{
  String name = id.toString();
  if (getChild(name)) {
    Fatal("tried to add client branch of existing name " << name);
  }
  addChild(Node::Ptr(new ClientBranch(id, this, server)));
}

void ClientsBranch::removeClient(ClientId id)
{
  String name = id.toString();
  if (!getChild(name)) {
    Fatal("tried to remove non-existant client branch " << name);
  }
  removeChild(name);
}

