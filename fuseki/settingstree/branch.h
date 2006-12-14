#ifndef FUSEKI__SETTINGSTREE__BRANCH_H
#define FUSEKI__SETTINGSTREE__BRANCH_H

#include <list>
#include "gnu_extensions.h"

#include "libsakusen-global.h"
#include "stringutils.h"
#include "clientid.h"
#include "settingstree/node.h"

namespace fuseki {
namespace settingsTree {

class Branch : public Node {
  protected:
    Branch(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server
      );
  private:
    __gnu_cxx::hash_map<String, Node::Ptr, sakusen::StringHash> children;

  protected:
    inline void addChild(Node::Ptr child) {
      children[child->getName()] = child;
    }
    inline void removeChild(String name) {
      assert(children.count(name));
      children.erase(name);
    }
    virtual Node::Ptr getNodeByListRef(std::list<String>& nodeAddress);
    virtual String changeRequestListRef(
        std::list<String>& setting,
        const String& value,
        const SettingsUser* user
      );
    virtual String getRequestListRef(
        std::list<String>& setting,
        String& value,
        Node::ConstPtr& node,
        const SettingsUser* user
      ) const;
  public:
    Node::Ptr getChild(String name);
      /* Returns Node::Ptr() if no such child */
    Node::ConstPtr getChild(String name) const;
      /* Returns Node::Ptr() if no such child */
};

}}

#endif // FUSEKI__SETTINGSTREE__BRANCH_H

