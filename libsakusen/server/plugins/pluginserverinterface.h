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
 * to the plugins at load time (when it calls PluginInfo::init) for them to
 * register their various components with the server.  See the descriptions of
 * specific methods on this interface for more details.
 */
class PluginServerInterface {
  public:
    virtual ~PluginServerInterface() {}

    /** \brief Should return true iff the game is already started */
    virtual bool isGameStarted() const = 0;

    /** \brief Should return a ResourceInterface
     *
     * This ResourceInterface should be one which the plugin can use to
     * load or save its data from or to whatever sakusen data directories have
     * been configured. */
    virtual ResourceInterface::Ptr getResourceInterface() const = 0;

    /** \brief Allows the plugin to register a listener
     *
     * This method allows the plugin to register a listener which should be
     * informed of various events as the game progresses (and possibly also
     * before the game begins).  See the Listener docs for more info. */
    virtual void registerListener(const Listener::VPtr&) const = 0;

    /** \name Option registration methods
     *
     * There are a variety of methods which plugins can use to register options
     * of various sorts.  The intent is that these should be used to configure
     * the behaviour of the plugin to requirements.  Possibly the server will
     * allow other clients to alter these option values also.
     *
     * Each option registration method specifies a name for the option, a
     * default value, and a pointer to as callback interface to be used when
     * the option value changes.  The server is free to ignore any and all of
     * these calls, so sensible defaults should be used wherever possible.
     *
     * The callback pointer should remain valid at least as long as the plugin
     * remains loaded.
     *
     * These methods have to be supplied individually, rather than as a single
     * template member because virtual template members are not allowed.
     */
    //@{
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
    //@}
    /** \brief Alternate signature for string option registration.
     *
     * We need this const char* version because otherwise the bool overload is
     * chosen in that context.  Note that this method is not virtual, and
     * automatically forwards to the String option registration function. */
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

