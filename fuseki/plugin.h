#ifndef PLUGIN_H
#define PLUGIN_H

#include "fuseki-global.h"

#include <stack>
#include <boost/variant.hpp>

#include "plugininfo.h"
#include "settingstree/leaf.h"
#include "settingstree/pluginbranch.h"
#include "optioncallback.h"

namespace fuseki {

class Server;

class Plugin :
  public sakusen::server::plugins::PluginServerInterface, boost::noncopyable {
  public:
    typedef boost::shared_ptr<Plugin> Ptr;
    typedef boost::shared_ptr<const Plugin> ConstPtr;

    Plugin(
        const String& name,
        const sakusen::server::plugins::PluginInfo::Ptr& info,
        Server* server
      );
    ~Plugin();
  private:
    const String name;
    sakusen::server::plugins::PluginInfo::Ptr info;
    Server* server;
    settingsTree::PluginBranch::Ptr pluginBranch;
    mutable std::stack<sakusen::server::Listener::VPtr> listeners;

    typedef boost::variant<
        sakusen::server::plugins::OptionCallback<String>*,
        sakusen::server::plugins::OptionCallback<std::set<String> >*,
        sakusen::server::plugins::OptionCallback<bool>*,
        sakusen::server::plugins::OptionCallback<uint8>*,
        sakusen::server::plugins::OptionCallback<sint8>*,
        sakusen::server::plugins::OptionCallback<uint16>*,
        sakusen::server::plugins::OptionCallback<sint16>*,
        sakusen::server::plugins::OptionCallback<uint32>*,
        sakusen::server::plugins::OptionCallback<sint32>*,
        sakusen::server::plugins::OptionCallback<uint64>*,
        sakusen::server::plugins::OptionCallback<sint64>*
      > OptionCallbacks;
    mutable sakusen::hash_map_string<OptionCallbacks>::type optionCallbacks;
  protected:
    bool isGameStarted() const;
    sakusen::ResourceInterface::Ptr getResourceInterface() const;
    void registerListener(const sakusen::server::Listener::VPtr&) const;
#define REGISTER_OPTION(type)                           \
    void registerOption(                                \
        const String& name,                             \
        sakusen::server::plugins::OptionCallback<type>* callback, \
        boost::call_traits<type>::param_type initialVal \
      ) const;
    REGISTER_OPTION(String)
    REGISTER_OPTION(std::set<String>)
    REGISTER_OPTION(bool)
    REGISTER_OPTION(sint8)
    REGISTER_OPTION(sint16)
    REGISTER_OPTION(sint32)
    REGISTER_OPTION(sint64)
    REGISTER_OPTION(uint8)
    REGISTER_OPTION(uint16)
    REGISTER_OPTION(uint32)
    REGISTER_OPTION(uint64)
#undef REGISTER_OPTION
  public:
    const String& getName() const { return name; }

    template<typename T>
    String settingAlterationCallback(
        settingsTree::Leaf* altering,
        typename boost::call_traits<T>::param_type newValue
      ) {
      sakusen::hash_map_string<OptionCallbacks>::type::iterator optionCallback =
        optionCallbacks.find(altering->getName());
      assert(optionCallback != optionCallbacks.end());
      return boost::get<sakusen::server::plugins::OptionCallback<T>*>(
          optionCallback->second
        )->setOption(
          optionCallback->first, newValue
        );
    }
};

}

#endif // PLUGIN_H

