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
    const Node* parent,
    Server* server
  ) :
  Node(name, readers, writers, parent, server)
{
}

Branch::~Branch()
{
  while (!children.empty()) {
    delete children.begin()->second;
    children.erase(children.begin());
  }
}

Node* Branch::getNodeByListRef(
    list<String>& nodeAddress)
{
  if (nodeAddress.empty()) {
    return this;
  }
  
  Node* child = getChild(nodeAddress.front());
  
  if (child == NULL) {
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

  Node* child = getChild(setting.front());

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
    const Node*& node,
    const SettingsUser* user) const
{
  if (!user->hasReadPermissionFor(this)) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }
  
  if (setting.empty()) {
    ostringstream out;
    out << static_cast<uint32>(children.size()) << " items";
    for (__gnu_cxx::hash_map<String, Node*, StringHash>::const_iterator
        child = children.begin(); child != children.end(); child++) {
      out << "\n" << child->second->getName();
    }
    value = out.str();
    node = this;
    return "";
  }

  const Node* child = getChild(setting.front());

  if (child == NULL) {
    return String("node '") + setting.front() + "' not found in '" +
      getFullName() + "'";
  }

  setting.pop_front();
  
  return child->getRequestListRef(setting, value, node, user);
}

Node* Branch::getChild(String name)
{
  hash_map<String, Node*, StringHash>::iterator child =
    children.find(name);

  if (child == children.end()) {
    return NULL;
  }

  return child->second;
}

const Node* Branch::getChild(String name) const
{
  hash_map<String, Node*, StringHash>::const_iterator child =
    children.find(name);

  if (child == children.end()) {
    return NULL;
  }

  return child->second;
}

