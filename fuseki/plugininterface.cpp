#include "plugininterface.h"

#include "fileutils.h"
#include "pluginexn.h"
#include "plugininfo.h"

#include <ltdl.h>

using namespace std;

using namespace sakusen::resources;
using namespace sakusen::server::plugins;
using namespace fuseki;

PluginInterface::PluginInterface(const list<String>& pP, Server* s) :
  pluginPaths(pP),
  server(s)
{}

Plugin::Ptr PluginInterface::load(const String& pluginName)
{
  for (list<String>::iterator path = pluginPaths.begin();
      path != pluginPaths.end(); ++path) {
    Debug("Looking for plugin " << pluginName << " in " << *path);
    list<String> candidates = fileUtils_findMatches(*path, pluginName);
    switch (candidates.size()) {
      case 0:
        break;
      case 1:
        {
          // We should have found a directory, and we want to open the module
          // therein
          String& candidate = candidates.front();
          String fullPluginName = fileUtils_notDirPart(candidate);
          String module = candidate + FILE_SEP + fullPluginName + ".la";
          lt_dlhandle moduleHandle = lt_dlopen(module.c_str());
          if (moduleHandle == NULL) {
            String error = "lt_dlopen(" + module + ") failed: " + lt_dlerror();
            throw PluginExn(error);
          }

          /* We could make the module resident
          if (lt_dlmakeresident(moduleHandle)) {
            String error =
              String("lt_dlmakeresident() failed: ") + lt_dlerror();
            throw PluginExn(error);
          }
           * ... but I'm not because it leaks a little memory. */

          String symbolName = "get_"+fullPluginName+"_info";
          lt_ptr symbol = lt_dlsym(moduleHandle, symbolName.c_str());

          if (symbol == NULL) {
            String error = "lt_dlsym(..., \"" + symbolName + "\") failed: " +
              lt_dlerror();
            throw PluginExn(error);
          }

          PluginInfo* (*infoSource)() =
            reinterpret_cast<PluginInfo* (*)()>(symbol);
          PluginInfo::Ptr info(infoSource());
          return Plugin::Ptr(new Plugin(fullPluginName, info, server));
        }
      default:
        throw DuplicatePluginExn(pluginName);
    }
  }
  throw PluginNotFoundExn(pluginName);
}

