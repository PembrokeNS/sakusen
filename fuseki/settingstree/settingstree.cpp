#include "settingstree.h"

#include "libsakusen-comms-global.h"
#include "stringutils.h"
#include "settingstree/serverbranch.h"
#include "settingstree/gamebranch.h"
#include "settingstree/playersbranch.h"

using namespace std;

using namespace fuseki::settingsTree;

SettingsTree::SettingsTree(Server* server) :
  Branch("", "world", "", NULL, server)
{
  addChild(new ServerBranch(this, server));
  addChild(new GameBranch(this, server));
  addChild(new ClientsBranch(this, server));
  addChild(new PlayersBranch(this, server));
}

SettingsTree::~SettingsTree()
{
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

Node* SettingsTree::getNode(const String& nodeAddress)
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
    const SettingsUser* user) const
{
  return getRequestList(
      stringNodeAddressToList(nodeAddress), value, user
    );
}

