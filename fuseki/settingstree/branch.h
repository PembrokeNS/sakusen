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
  public:
    typedef boost::shared_ptr<Branch> Ptr;
    typedef boost::shared_ptr<const Branch> ConstPtr;
  protected:
    Branch(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server
      );
  private:
    sakusen::hash_map_string<Node::Ptr>::type children;

  protected:
    Node::Ptr addChild(Node::Ptr child);
    void removeChild(String name);
    virtual Node::Ptr getNodeByListRef(std::list<String>& nodeAddress);
    virtual String changeRequestListRef(
        std::list<String>& setting,
        const String& value,
        const SettingsUser* user
      );
    virtual boost::tuple<String, std::set<String>, Node::ConstPtr>
      getRequestListRef(
        std::list<String>& nodeAddress,
        const SettingsUser* user
      ) const;
  public:
    bool isLeaf() const { return false; }
    std::set<String> getChildNames() const;
    /* Returns Node::Ptr() if no such child */
    Node::Ptr getChild(String name);
    /* Returns Node::ConstPtr() if no such child */
    Node::ConstPtr getChild(String name) const;
};

}}

#endif // FUSEKI__SETTINGSTREE__BRANCH_H

