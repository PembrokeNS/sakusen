#include "settingstree/branch.h"

#include "server.h"
#include "null_deleter.h"

using namespace std;

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

Node::Ptr Branch::addChild(Node::Ptr child) {
  children[child->getName()] = child;
  /* Can't use ptrToThis() because might still be constructing Server */
  server->settingAlteredCallback(Node::Ptr(this, null_deleter()));
  return child;
}

void Branch::removeChild(String name) {
  assert(children.count(name));
  children.erase(name);
  server->settingAlteredCallback(ptrToThis());
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
  if (!user->hasReadPermissionFor(ptrToThis())) {
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

boost::tuple<String, std::set<String>, Node::ConstPtr>
Branch::getRequestListRef(
    std::list<String>& nodeAddress,
    const SettingsUser* user
  ) const
{
  if (!user->hasReadPermissionFor(ptrToThis())) {
    return String("cannot read node '") + getFullName() +
      "': permission denied";
  }
  
  if (nodeAddress.empty()) {
    return boost::make_tuple("", getChildNames(), ptrToThis());
  }

  Node::ConstPtr child = getChild(nodeAddress.front());

  if (!child) {
    return boost::make_tuple(
        String("node '") + nodeAddress.front() + "' not found in '" +
        getFullName() + "'", set<String>(), Ptr()
      );
  }

  nodeAddress.pop_front();
  
  return child->getRequestListRef(nodeAddress, user);
}

set<String> Branch::getChildNames() const
{
  set<String> childNames;
  for (u_map<String, Node::Ptr>::type::const_iterator
      child = children.begin(); child != children.end(); child++) {
    childNames.insert(child->second->getName());
  }
  return childNames;
}

Node::Ptr Branch::getChild(String name)
{
  u_map<String, Node::Ptr>::type::iterator child =
    children.find(name);

  if (child == children.end()) {
    return Node::Ptr();
  }

  return child->second;
}

Node::ConstPtr Branch::getChild(String name) const
{
  u_map<String, Node::Ptr>::type::const_iterator child =
    children.find(name);

  if (child == children.end()) {
    return Node::ConstPtr();
  }

  return child->second;
}

