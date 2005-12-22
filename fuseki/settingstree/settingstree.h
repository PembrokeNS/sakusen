#ifndef SETTINGSTREE_H
#define SETTINGSTREE_H

#include <list>

#include "libsakusen-global.h"
#include "clientid.h"
#include "settingstree/branch.h"
#include "settingstree/clientsbranch.h"
#include "settingsuser.h"

namespace fuseki {
namespace settingsTree {

class SettingsTree : public Branch {
  private:
    SettingsTree();
    SettingsTree(const SettingsTree&);
  public:
    SettingsTree(Server* server);
    ~SettingsTree();
  private:
    std::list<String> stringNodeAddressToList(const String& nodeAddress) const;
  public:
    String changeRequest(
        const String& node,
        const String& value,
        const SettingsUser* user
      );
    String getRequest(
        const String& node,
        String& value,
        const SettingsUser* user
      ) const;
    Node* getNode(const String& nodeAddress);
    inline ClientsBranch* getClientsBranch() {
      return dynamic_cast<ClientsBranch*>(getNode("clients"));
    }
};

}}

#endif // SETTINGSTREE_H

