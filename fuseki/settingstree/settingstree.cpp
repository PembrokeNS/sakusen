#include "settingstree.h"

#include "libsakusen-comms-global.h"
#include "stringutils.h"
#include "settingstree/serverbranch.h"
#include "settingstree/gamebranch.h"
#include "settingstree/playersbranch.h"

using namespace std;

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

SettingsTree::SettingsTree(Server* server) :
  Branch("", "world", "", NULL, server)
{
  addChild(Node::Ptr(new ServerBranch(this, server)));
  addChild(Node::Ptr(new GameBranch(this, server)));
  addChild(Node::Ptr(new ClientsBranch(this, server)));
  addChild(Node::Ptr(new PlayersBranch(this, server)));
}

list<String> SettingsTree::stringNodeAddressToList(
    const String& nodeAddress
  ) const
{
  list<String> addressAsList =
    stringUtils_split(nodeAddress, SETTINGS_DELIMITER);
  /* Remove effect of leading, trailing or duplicate delimiters */
  list<String>::iterator name = addressAsList.begin();
  while (name != addressAsList.end()) {
    if (name->empty()) {
      name = addressAsList.erase(name);
    } else {
      name++;
    }
  }
  return addressAsList;
}

Node::Ptr SettingsTree::getNode(const String& nodeAddress)
{
  return getNodeByList(stringNodeAddressToList(nodeAddress));
}

String SettingsTree::changeRequest(
    const String& nodeAddress,
    const String& value,
    const SettingsUser* user)
{
  return changeRequestList(
      stringNodeAddressToList(nodeAddress), value, user
    );
}

String SettingsTree::getRequest(
    const String& nodeAddress,
    String& value,
    Node::ConstPtr& node,
    const SettingsUser* user) const
{
  return getRequestList(
      stringNodeAddressToList(nodeAddress), value, node, user
    );
}

