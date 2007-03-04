#include "plugin.h"

#include "settingstree/pluginbranch-methods.h"
#include "server.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::server;
using namespace sakusen::server::plugins;
using namespace fuseki;

Plugin::Plugin(const String& n, const PluginInfo::Ptr& i, Server* s) :
  name(n),
  info(i),
  server(s)
{
  QDebug("Adding PluginBranch " << name);
  pluginBranch =
    server->getSettings()->getPluginsBranch()->addPlugin(name, this);
  info->init(*this);
}

Plugin::~Plugin()
{
  while (!listeners.empty()) {
    server->unregisterListener(listeners.top());
    listeners.pop();
  }
  QDebug("Removing PluginBranch " << name);
  server->getSettings()->getPluginsBranch()->removePlugin(name);
  info->close();
}

bool Plugin::isGameStarted() const
{
  return server->getGameStarted();
}

ResourceInterface::Ptr Plugin::getResourceInterface() const
{
  return server->getResourceInterface();
}

void Plugin::registerListener(const Listener::VPtr& listener) const
{
  listeners.push(listener);
  server->registerListener(listener);
}

#define REGISTER_OPTION(type)                    \
void Plugin::registerOption(                     \
    const String& name,                          \
    OptionCallback<type>* callback,              \
    boost::call_traits<type>::param_type initialVal \
  ) const                                        \
{                                                \
  pluginBranch->addLeaf<type>(name, initialVal); \
  assert(0 == optionCallbacks.count(name));      \
  optionCallbacks[name] = callback;              \
}

REGISTER_OPTION(String)
REGISTER_OPTION(set<String>)
REGISTER_OPTION(bool)
REGISTER_OPTION(sint8)
REGISTER_OPTION(sint16)
REGISTER_OPTION(sint32)
REGISTER_OPTION(sint64)
REGISTER_OPTION(uint8)
REGISTER_OPTION(uint16)
REGISTER_OPTION(uint32)
REGISTER_OPTION(uint64)

#undef REGISTER_INT_OPTION

