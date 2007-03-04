#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include "libsakusen-global.h"

#include <list>

#include "plugin.h"

namespace fuseki {

class PluginInterface {
  public:
    PluginInterface(
        const std::list<String>& pluginPaths,
        Server* server
      );
  private:
    std::list<String> pluginPaths;
    Server* server;
  public:
    Plugin::Ptr load(const String& pluginName);
};

}

#endif // PLUGININTERFACE_H

