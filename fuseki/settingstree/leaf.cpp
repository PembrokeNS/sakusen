#include "settingstree/leaf.h"

#include "server.h"

using namespace std;

using namespace fuseki;
using namespace fuseki::settingsTree;

Leaf::Leaf(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server
  ) :
  Node(name, readers, writers, parent, server)
{
}

Leaf::~Leaf()
{
}

Node::Ptr Leaf::getNodeByListRef(list<String>& nodeAddress)
{
  if (!nodeAddress.empty()) {
    SAKUSEN_FATAL("sought child of leaf '" << getFullName() << "'");
  }

  return ptrToThis();
}

String Leaf::changeRequestListRef(
    list<String>& nodeAddress,
    const String& value,
    const SettingsUser* user)
{
  //SAKUSEN_DEBUG("checking permissions for node " << getFullName());

  if (!user->hasReadPermissionFor(ptrToThis())) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }

  if (!nodeAddress.empty()) {
    return String("node '") + getFullName() + "' is a leaf and has no child";
  }

  if (!user->hasWritePermissionFor(ptrToThis())) {
    return String("cannot write to node '") + getFullName() +
      "': permission denied";
  }

  String reason = setValue(value);

  if (reason == "") {
    server->settingAlteredCallback(ptrToThis());
  }

  return reason;
}

boost::tuple<String, std::set<String>, Node::ConstPtr>
Leaf::getRequestListRef(
    std::list<String>& nodeAddress,
    const SettingsUser* user
  ) const
{
  if (!user->hasReadPermissionFor(ptrToThis())) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }

  if (!nodeAddress.empty()) {
    return String("node '") + getFullName() + "' is a leaf and has no child";
  }

  return boost::make_tuple("", getValue(), ptrToThis());
}

