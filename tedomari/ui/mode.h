#ifndef TEDOMARI__UI__MODE_H
#define TEDOMARI__UI__MODE_H

#include "gnu_extensions.h"
#include "stringutils.h"

#include "ui/modifiedkeyevent.h"
#include "ui/command.h"
#include "ui/function.h"

namespace tedomari {
namespace ui {

/** \brief Represents a possible state of the UI
 *
 * A Mode describes a way for the UI to respond to input.  It includes such
 * things as bindings of keys to Commands, and Commands themselves. */
class Mode {
  private:
    String name;
    u_map<
        ModifiedKeyEvent, std::list<String>, ModifiedKeyEventHash
      >::type bindings;
    u_map<String, Command>::type commands;

  public:
    inline const String& getName() const { return name; }
    inline const u_map<
        ModifiedKeyEvent, std::list<String>, ModifiedKeyEventHash
      >::type& getBindings() const
    {
      return bindings;
    }
    inline const u_map<String, Command>::type&
      getCommands() const {
      return commands;
    }
    void addCommand(const String& name, const Command& cmd, UI* ui);
    void addBinding(const ModifiedKeyEvent& key, const std::list<String>& cmd);
    void addFunction(const Function& function, UI* ui);
    
  private:
    static Mode getCommon(UI* ui);
  public:
    static Mode getDefault(const String& name, UI* ui);
    static Mode getNormal(UI* ui);
    static Mode getUnit(UI* ui);
    static Mode getTarget(UI* ui);
    static Mode getDefault(UI* ui);
};

}}

#endif // TEDOMARI__UI__MODE_H

