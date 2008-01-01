#ifndef LEAF_H
#define LEAF_H

#include "settingstree/node.h"
#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class Leaf : public Node {
  public:
    Leaf(
        const String& name,
        const String& readers,
        const String& writers,
        Branch* parent,
        Server* server
      );
    virtual ~Leaf();
  protected:
    virtual Node::Ptr getNodeByListRef(
        std::list<String>& nodeAddress
      );
    virtual String changeRequestListRef(
        std::list<String>& nodeAddress,
        const String& value,
        const SettingsUser* client
      );
    virtual boost::tuple<String, std::set<String>, Node::ConstPtr>
      getRequestListRef(
        std::list<String>& nodeAddress,
        const SettingsUser* client
      ) const;
    virtual String setValue(const String& v) = 0;
  public:
    bool isLeaf() const { return true; }
    virtual std::set<String> getValue() const = 0;
};

}}

#endif // LEAF_H

