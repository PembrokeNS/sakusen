#include "ui/mode.h"

#include "ui/ui.h"

using namespace __gnu_cxx;

using namespace sakusen;
using namespace tedomari::ui;

void Mode::addCommand(const String& name, const Command& cmd, UI* ui)
{
  assert(!name.empty());

  hash_map<String, Command, StringHash>::iterator existing =
    commands.find(name);
  if (existing == commands.end()) {
    /* No command of that name already exists, so we can proceed */
    commands[name] = cmd;
    /* Add also all non-ambiguous abbreviations of the command */
    String abbrev(name.begin(), name.end()-1);
    String ambiguity;
    bool ambiguous = false;
    while (!abbrev.empty()) {
      existing = commands.find(abbrev);
      if (existing == commands.end()) {
        if (ambiguous) {
          assert(false);
        } else {
          commands[abbrev] = Command(name);
        }
      } else {
        switch (existing->second.getType()) {
          case commandType_abbreviation:
            if (ambiguous) {
              existing->second = Command(ambiguity);
            } else {
              existing->second = Command();
              ambiguity = abbrev;
              ambiguous = true;
            }
            break;
          default:
            return;
        }
      }
      abbrev.erase(abbrev.end()-1, abbrev.end());
    }
  } else {
    /* Command of that name already exists */
    switch (existing->second.getType()) {
      case commandType_ambiguous:
        existing->second = cmd;
        return;
      case commandType_abbreviation:
        {
          /* Replace the existing command with the new one */
          existing->second = cmd;
          /* Fix all abbreviations down to the next branch */
          String abbrev(name.begin(), name.end()-1);
          while (!abbrev.empty()) {
            existing = commands.find(abbrev);
            assert(existing != commands.end());
            
            if (existing->second.getType() == commandType_abbreviation) {
              existing->second = Command(name);
            } else {
              break;
            }
            abbrev.erase(abbrev.end()-1, abbrev.end());
          }
        }
        return;
      default:
        ui->alert("command of name '" + name + "' already exists");
        return;
    }
  }
}

void Mode::addBinding(const ModifiedKeyEvent& key, const std::list<String>& cmd)
{
  if (bindings.count(key)) {
    Debug("replacing existing binding");
  }
  bindings[key] = cmd;
}

void Mode::addFunction(const Function& function, UI* ui)
{
  addCommand(function.getName(), Command(function), ui);
}

#define ADD_COMMAND(name) \
  m.addCommand(#name, Command(builtinCommand_ ## name, #name), ui)

Mode Mode::getCommon(UI* ui)
{
  Mode m;

  ADD_COMMAND(alias);
  ADD_COMMAND(bind);
  ADD_COMMAND(func);
  ADD_COMMAND(switchmode);
  ADD_COMMAND(alert);
  ADD_COMMAND(prompt);
  ADD_COMMAND(quit);
  ADD_COMMAND(movemaprelfrac);
  ADD_COMMAND(startdragregion);
  ADD_COMMAND(stopdragregion);

  return m;
}

Mode Mode::getNormal(UI* ui)
{
  Mode m(getCommon(ui));

  return m;
}

Mode Mode::getUnit(UI* ui)
{
  Mode m(getCommon(ui));

  ADD_COMMAND(select);
  ADD_COMMAND(move);

  return m;
}

#undef ADD_COMMAND

