#ifndef LIBSAKUSEN_SERVER_PLUGINS__PLUGINEXN_H
#define LIBSAKUSEN_SERVER_PLUGINS__PLUGINEXN_H

#include <sakusen/exceptions.h>

namespace sakusen {
namespace server {
namespace plugins {

/** \brief Exception for use by plugins.
 *
 * Plugins should throw exceptions which are instances either of this class, or
 * of classes derived from it */
class PluginExn : public Exn {
  public:
    PluginExn(const String& message) : Exn(message) {}
};

class PluginNotFoundExn : public PluginExn {
  public:
    PluginNotFoundExn(const String& name) :
      PluginExn("plugin '"+name+"' not found")
    {}
};

class DuplicatePluginExn : public PluginExn {
  public:
    DuplicatePluginExn(const String& name) :
      PluginExn("name '"+name+"' does not uniquely specify a plugin")
    {}
};

}}}

#endif // LIBSAKUSEN_SERVER_PLUGINS__PLUGINEXN_H

