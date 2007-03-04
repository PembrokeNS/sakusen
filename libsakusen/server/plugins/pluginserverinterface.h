#ifndef PLUGINSERVERINTERFACE_H
#define PLUGINSERVERINTERFACE_H

#include <set>

#include "listener.h"
#include "optioncallback.h"

namespace sakusen {
namespace server {
namespace plugins {

/** \brief Interface to the server for plugins
 *
 * A server which wishes to load standard plugins must provide this interface
 * to the plugins at load time for them to register their various components
 * with the server.
 */
class PluginServerInterface {
  public:
    virtual ~PluginServerInterface() {}

    virtual bool isGameStarted() const = 0;
    virtual ResourceInterface::Ptr getResourceInterface() const = 0;
    virtual void registerListener(const Listener::VPtr&) const = 0;
#define REGISTER_OPTION(type)                           \
    virtual void registerOption(                        \
        const String& name,                             \
        OptionCallback<type>* callback,                 \
        boost::call_traits<type>::param_type initialVal \
      ) const = 0;
    REGISTER_OPTION(String)
    REGISTER_OPTION(std::set<String>)
    REGISTER_OPTION(bool);
    REGISTER_OPTION(sint8);
    REGISTER_OPTION(sint16);
    REGISTER_OPTION(sint32);
    REGISTER_OPTION(sint64);
    REGISTER_OPTION(uint8);
    REGISTER_OPTION(uint16);
    REGISTER_OPTION(uint32);
    REGISTER_OPTION(uint64);
#undef REGISTER_OPTION
    /* Also need a const char* version because otherwise the bool overload is
     * chosen in that context */
    void registerOption(
        const String& name,
        OptionCallback<String>* callback,
        const char* initialVal
      ) const {
      registerOption(name, callback, String(initialVal));
    }
};

}}}

#endif // PLUGINSERVERINTERFACE_H

