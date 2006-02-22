#ifndef UI__MODE_H
#define UI__MODE_H

#include "gnu_extensions.h"
#include "stringutils.h"

#include "ui/modifiedkey.h"
#include "ui/command.h"

namespace tedomari {
namespace ui {

class Mode {
  private:
    __gnu_cxx::hash_map<ModifiedKey, String, ModifiedKeyHash> bindings;
    __gnu_cxx::hash_map<String, Command, sakusen::StringHash> commands;

  public:
    inline const __gnu_cxx::hash_map<ModifiedKey, String, ModifiedKeyHash>&
      getBindings() const
    {
      return bindings;
    }
    inline const __gnu_cxx::hash_map<String, Command, sakusen::StringHash>&
      getCommands() const {
      return commands;
    }
    void addCommand(const String& name, const Command& cmd, UI* ui);
    void addBinding(const ModifiedKey& key, const String& cmd);
    
    static Mode getNormal(UI* ui);
};

}}

#endif // UI__MODE_H

