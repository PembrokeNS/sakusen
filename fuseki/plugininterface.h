#ifndef FUSEKI__PLUGININTERFACE_H
#define FUSEKI__PLUGININTERFACE_H

#include "libsakusen-global.h"

#include <vector>
#include <boost/filesystem.hpp>

#include "plugin.h"

namespace fuseki {

/** \brief Provides an interface to available plugins for the server
 *
 * This class can be used by the server to seek out and load plugins n request.
 */
class PluginInterface {
  public:
    /** \brief Standard constructor
     *
     * \param pluginPaths List of paths in which to search for plugins
     * \param server      Server that will be doing the loading
     */
    PluginInterface(
        const std::vector<boost::filesystem::path>& pluginPaths,
        Server* server
      );
  private:
    std::vector<boost::filesystem::path> pluginPaths;
    Server* server;
  public:
    Plugin::Ptr load(const String& pluginName);
};

}

#endif // FUSEKI__PLUGININTERFACE_H

