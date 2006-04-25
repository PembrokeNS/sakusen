#ifndef LEAF_H
#define LEAF_H

#include "settingstree/node.h"
#include "settingstree/branch.h"

namespace fuseki {
namespace settingsTree {

class Leaf : public Node {
  private:
    Leaf();
    Leaf(const Leaf&);
  public:
    Leaf(
        const String& name,
        const String& readers,
        const String& writers,
        const Branch* parent,
        Server* server
      );
    virtual ~Leaf();
  protected:
    virtual Node* getNodeByListRef(
        std::list<String>& nodeAddress
      );
    virtual String changeRequestListRef(
        std::list<String>& nodeAddress,
        const String& value,
        const SettingsUser* client
      );
    virtual String getRequestListRef(
        std::list<String>& nodeAddress,
        String& value,
        const Node*& node,
        const SettingsUser* client
      ) const;
    virtual String setValue(const String& v) = 0;
  public:
    virtual String getValue() const = 0;
};

}}

#endif // LEAF_H

