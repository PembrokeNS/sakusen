#ifndef FUSEKI__SETTINGSTREE__SETTINGSTREE_H
#define FUSEKI__SETTINGSTREE__SETTINGSTREE_H

#include <list>

#include "libsakusen-global.h"
#include "clientid.h"
#include "settingstree/branch.h"
#include "settingstree/serverbranch.h"
#include "settingstree/clientsbranch.h"
#include "settingstree/playersbranch.h"
#include "settingstree/pluginsbranch.h"
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
    boost::tuple<String, std::set<String>, Node::ConstPtr> getRequest(
        const String& nodeAddress,
        const SettingsUser* user
      ) const;
    Node::Ptr getNode(const String& nodeAddress);
    inline ServerBranch::Ptr getServerBranch() {
      return boost::dynamic_pointer_cast<ServerBranch>(
          getNode("server")
        );
    }
    inline ClientsBranch::Ptr getClientsBranch() {
      return boost::dynamic_pointer_cast<ClientsBranch>(
          getNode("clients")
        );
    }
    inline PlayersBranch::Ptr getPlayersBranch() {
      return boost::dynamic_pointer_cast<PlayersBranch>(
          getNode("players")
        );
    }
    inline PluginsBranch::Ptr getPluginsBranch() {
      return boost::dynamic_pointer_cast<PluginsBranch>(
          getNode("plugins")
        );
    }
};

}}

#endif // FUSEKI__SETTINGSTREE__SETTINGSTREE_H

