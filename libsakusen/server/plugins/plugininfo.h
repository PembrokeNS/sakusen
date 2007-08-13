#ifndef PLUGININFO_H
#define PLUGININFO_H

#include <boost/utility.hpp>

#include "pluginserverinterface.h"

namespace sakusen {
namespace server {
namespace plugins {

/** \brief Class which provides the necessary information about a plugin to a
 * server.
 *
 * Certain plugins (such as the replay recorder) may be suitable for use in
 * more than one server, but don't quite qualify for inclusion in the core
 * server library.  They can be implemented as an independent dlopenable
 * module, and provide the server with the necessary information about
 * themselves by supplying a PluginInfo object.
 *
 * They should do this by defining a function with C-style linkage (extern "C")
 * called get_PLUGIN_info where PLUGIN is the name of the plugin (the same as
 * the name of the library excluding .so) which takes no arguments and returns
 * a PluginInfo* pointing to info for that plugin.  See replay_recorder for an
 * example.
 * */
class PluginInfo : boost::noncopyable {
  public:
    typedef boost::shared_ptr<PluginInfo> Ptr;
    typedef boost::shared_ptr<const PluginInfo> ConstPtr;
  protected:
    PluginInfo() {}
  public:
    virtual ~PluginInfo() {}
	//interface is an MS-specific keyword.
    virtual void init(PluginServerInterface& PSinterface) = 0;
    virtual void close() = 0;
};

}}}

#endif // PLUGININFO_H

