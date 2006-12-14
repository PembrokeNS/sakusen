#ifndef SETTINGSTREE_H
#define SETTINGSTREE_H

#include <list>

#include "libsakusen-global.h"
#include "clientid.h"
#include "settingstree/branch.h"
#include "settingstree/clientsbranch.h"
#include "settingstree/playersbranch.h"
#include "settingsuser.h"

namespace fuseki {
namespace settingsTree {

class SettingsTree : public Branch {
  public:
    typedef boost::shared_ptr<SettingsTree> Ptr;

    SettingsTree(Server* server);
  private:
    std::list<String> stringNodeAddressToList(const String& nodeAddress) const;
  public:
    String changeRequest(
        const String& node,
        const String& value,
        const SettingsUser* user
      );
    String getRequest(
        const String& nodeAddress,
        String& value,
        Node::ConstPtr& node,
        const SettingsUser* user
      ) const;
    Node::Ptr getNode(const String& nodeAddress);
    inline ClientsBranch::Ptr getClientsBranch() {
      return boost::dynamic_pointer_cast<ClientsBranch, Node>(
          getNode("clients")
        );
    }
    inline PlayersBranch::Ptr getPlayersBranch() {
      return boost::dynamic_pointer_cast<PlayersBranch, Node>(
          getNode("players")
        );
    }
};

}}

#endif // SETTINGSTREE_H

