#include "settingstree/leaf.h"

#include "server.h"

using namespace std;

using namespace fuseki;
using namespace fuseki::settingsTree;

Leaf::Leaf(
    const String& name,
    const String& readers,
    const String& writers,
    const Branch* parent,
    Server* server
  ) :
  Node(name, readers, writers, parent, server)
{
}

Leaf::~Leaf()
{
}

Node* Leaf::getNodeByListRef(list<String>& nodeAddress)
{
  if (!nodeAddress.empty()) {
    Fatal("sought child of leaf '" << getFullName() << "'");
  }

  return this;
}

String Leaf::changeRequestListRef(
    list<String>& nodeAddress,
    const String& value,
    const SettingsUser* user)
{
  //Debug("checking permissions for node " << getFullName());
  
  if (!user->hasReadPermissionFor(this)) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }
  
  if (!nodeAddress.empty()) {
    return String("node '") + getFullName() + "' is a leaf and has no child";
  }
  
  if (!user->hasWritePermissionFor(this)) {
    return String("cannot write to node '") + getFullName() +
      "': permission denied";
  }

  String reason = setValue(value);

  if (reason == "") {
    server->settingAlteredCallback(this);
  }

  return reason;
}

String Leaf::getRequestListRef(
    list<String>& nodeAddress,
    String& value,
    const SettingsUser*) const
{
  if (!nodeAddress.empty()) {
    return String("node '") + getFullName() + "' is a leaf and has no child";
  }
  
  /* TODO: check permissions of user */

  value = getValue();
  return "";
}

