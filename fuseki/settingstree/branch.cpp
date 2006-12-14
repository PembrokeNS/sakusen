#include "settingstree/branch.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace fuseki;
using namespace fuseki::settingsTree;

Branch::Branch(
    const String& name,
    const String& readers,
    const String& writers,
    Branch* parent,
    Server* server
  ) :
  Node(name, readers, writers, parent, server)
{
}

Node::Ptr Branch::getNodeByListRef(
    list<String>& nodeAddress
  )
{
  if (nodeAddress.empty()) {
    return ptrToThis();
  }

  Node::Ptr child = getChild(nodeAddress.front());
  
  if (!child) {
    Fatal("node '" << nodeAddress.front() << "' not found in '" <<
        getFullName() << "'");
  }

  nodeAddress.pop_front();
  
  return child->getNodeByListRef(nodeAddress);
}

String Branch::changeRequestListRef(
    list<String>& setting,
    const String& value,
    const SettingsUser* user)
{
  if (!user->hasReadPermissionFor(this)) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }
  
  if (setting.empty()) {
    return "requested node '" + getFullName() + "' not a leaf";
  }

  Node::Ptr child = getChild(setting.front());

  if (child == NULL) {
    return String("node '") + setting.front() + "' not found in '" +
      getFullName() + "'";
  }

  setting.pop_front();
  
  return child->changeRequestListRef(setting, value, user);
}

String Branch::getRequestListRef(
    list<String>& setting,
    String& value,
    Node::ConstPtr& node,
    const SettingsUser* user) const
{
  if (!user->hasReadPermissionFor(this)) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }
  
  if (setting.empty()) {
    ostringstream out;
    out << static_cast<uint32>(children.size()) << " items";
    for (__gnu_cxx::hash_map<String, Node::Ptr, StringHash>::const_iterator
        child = children.begin(); child != children.end(); child++) {
      out << "\n" << child->second->getName();
    }
    value = out.str();
    node = ptrToThis();
    return "";
  }

  Node::ConstPtr child = getChild(setting.front());

  if (!child) {
    return String("node '") + setting.front() + "' not found in '" +
      getFullName() + "'";
  }

  setting.pop_front();
  
  return child->getRequestListRef(setting, value, node, user);
}

Node::Ptr Branch::getChild(String name)
{
  hash_map<String, Node::Ptr, StringHash>::iterator child =
    children.find(name);

  if (child == children.end()) {
    return Node::Ptr();
  }

  return child->second;
}

Node::ConstPtr Branch::getChild(String name) const
{
  hash_map<String, Node::Ptr, StringHash>::const_iterator child =
    children.find(name);

  if (child == children.end()) {
    return Node::ConstPtr();
  }

  return child->second;
}

