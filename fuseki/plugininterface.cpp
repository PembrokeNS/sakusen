

#ifdef __GNUC__
  #ifdef ENABLE_LTDL_HACKED
    #include <ltdl_hacked/ltdl_hacked.h>
  #else
    #include <ltdl.h>
  #endif
#endif //The Guild does not condone ltdl

#include <sakusen/fileutils.h>
#include <sakusen/server/plugins/pluginexn.h>
#include <sakusen/server/plugins/plugininfo.h>
#include "plugininterface.h"

using namespace std;

using namespace sakusen::server::plugins;
using namespace fuseki;

/*This MUST be after all the other headers, due to name conflicts*/
#ifdef WIN32
  #include <windows.h>
#endif

PluginInterface::PluginInterface(
    const vector<boost::filesystem::path>& pP,
    Server* s
  ) :
  pluginPaths(pP),
  server(s)
{}

/** \brief Attempt to find and load a plugin of a given name
 *
 * Looks for a plugin of the given name (the name need only be a prefix of the
 * full name).  If it finds it, it tries to load it, create a Plugin object to
 * wrap in, and return that.  If anything goes wrong, a PluginExn is thrown.
 */
Plugin::Ptr PluginInterface::load(const String& pluginName)
{
  for (vector<boost::filesystem::path>::iterator path = pluginPaths.begin();
      path != pluginPaths.end(); ++path) {
    SAKUSEN_DEBUG(
        "Looking for plugin " << pluginName << " in " <<
        path->string()
      );
    list<boost::filesystem::path> candidates =
      sakusen::fileUtils_findMatches(*path, pluginName);
    switch (candidates.size()) {
      case 0:
        break;
      case 1:
        {
          // We should have found a directory, and we want to open the module
          // therein
          boost::filesystem::path& candidate = candidates.front();
          String fullPluginName = boost::filesystem::basename(candidate.filename());
#ifdef __GNUC__
          boost::filesystem::path module =
            candidate / (fullPluginName + ".la");
          lt_dlhandle moduleHandle =
            lt_dlopen(module.string().c_str());
          if (moduleHandle == NULL) {
            String error = "lt_dlopen(" + module.string() +
              ") failed: " + lt_dlerror();
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
#else
          
          //Opens the plugin.
          boost::filesystem::path module = candidate;
          //Equivalent to  lt_dlhandle moduleHandle = lt_dlopen(module.c_str());
          /** \bug This should work for UNICODE filenames. */
          HMODULE moduleHandle =
            LoadLibraryA(module.file_string().c_str());
          //Error handling for the above.
          if(moduleHandle == NULL) {
            char buffer[33];
            _itoa_s(GetLastError(), buffer, 33,10);
            String error =
              "LoadLibrary() failed. Error value: " + String(buffer)+
              "\nString Searched for: "+module.file_string();
            throw PluginExn(error);
          }

          String symbolName = "get_"+fullPluginName+"_info";
          //Equivalent to  lt_ptr symbol = lt_dlsym(moduleHandle, symbolName.c_str());
          FARPROC symbol = GetProcAddress(moduleHandle,symbolName.c_str());
          //Error handling if this fails.
          if(symbol==NULL)
          {
            char buffer[33];
            _itoa_s(GetLastError(), buffer, 33,10);
            String error = "GetProcAddress() on "+symbolName+" in " + candidate.string()+
              " failed. Error value: " + String(buffer);
            throw PluginExn(error);
          }
#endif //__GNUC__

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

