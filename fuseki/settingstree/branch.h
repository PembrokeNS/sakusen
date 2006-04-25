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
  private:
    Branch();
    Branch(const Branch&);
  protected:
    Branch(
        const String& name,
        const String& readers,
        const String& writers,
        const Node* parent,
        Server* server
      );
  public:
    virtual ~Branch();
  private:
    __gnu_cxx::hash_map<String, Node*, sakusen::StringHash> children;

  protected:
    /* Calling this method transfers ownership of child to this */
    inline void addChild(Node* child) {
      children[child->getName()] = child;
    }
    inline void removeChild(String name) {
      assert(children.count(name));
      delete children[name];
      children.erase(name);
    }
    virtual Node* getNodeByListRef(std::list<String>& nodeAddress);
    virtual String changeRequestListRef(
        std::list<String>& setting,
        const String& value,
        const SettingsUser* user
      );
    virtual String getRequestListRef(
        std::list<String>& setting,
        String& value,
        const Node*& node,
        const SettingsUser* user
      ) const;
  public:
    Node* getChild(String name);
      /* Returns NULL if no such child */
    const Node* getChild(String name) const;
      /* Returns NULL if no such child */
};

}}

#endif // FUSEKI__SETTINGSTREE__BRANCH_H

