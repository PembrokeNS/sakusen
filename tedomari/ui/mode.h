#ifndef UI__MODE_H
#define UI__MODE_H

#include "gnu_extensions.h"
#include "stringutils.h"

#include "ui/modifiedkeyevent.h"
#include "ui/command.h"
#include "ui/function.h"

namespace tedomari {
namespace ui {

class Mode {
  private:
    __gnu_cxx::hash_map<
        ModifiedKeyEvent, std::list<String>, ModifiedKeyEventHash
      > bindings;
    __gnu_cxx::hash_map<String, Command, sakusen::StringHash> commands;

  public:
    inline const __gnu_cxx::hash_map<
        ModifiedKeyEvent, std::list<String>, ModifiedKeyEventHash
      >& getBindings() const
    {
      return bindings;
    }
    inline const __gnu_cxx::hash_map<String, Command, sakusen::StringHash>&
      getCommands() const {
      return commands;
    }
    void addCommand(const String& name, const Command& cmd, UI* ui);
    void addBinding(const ModifiedKeyEvent& key, const std::list<String>& cmd);
    void addFunction(const Function& function, UI* ui);
    
  private:
    static Mode getCommon(UI* ui);
  public:
    static Mode getNormal(UI* ui);
    static Mode getUnit(UI* ui);
};

}}

#endif // UI__MODE_H

