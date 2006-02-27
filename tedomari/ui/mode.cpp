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
    String abbrev(name.begin(), --name.end());
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
      abbrev.erase(--abbrev.end(), abbrev.end());
    }
  } else {
    /* Command of that name already exists */
    switch (existing->second.getType()) {
      case commandType_ambiguous:
        existing->second = cmd;
        return;
      case commandType_abbreviation:
        {
          /* Fix all abbreviations down to the next branch */
          String abbrev(name.begin(), --name.end());
          while (!abbrev.empty()) {
            existing = commands.find(abbrev);
            assert(existing != commands.end());
            
            if (existing->second.getType() == commandType_abbreviation) {
              existing->second = Command(name);
            } else {
              break;
            }
            abbrev.erase(--abbrev.end(), abbrev.end());
          }
        }
        return;
      default:
        ui->alert("command of name '" + name + "' already exists");
        return;
    }
  }
}

void Mode::addBinding(const ModifiedKey& key, const String& cmd)
{
  if (bindings.count(key)) {
    Debug("replacing existing binding");
  }
  bindings[key] = cmd;
}

Mode Mode::getNormal(UI* ui)
{
  Mode m;

  m.addCommand("alias",      Command(builtinCommand_alias), ui);
  m.addCommand("bind",       Command(builtinCommand_bind), ui);
  m.addCommand("alert",      Command(builtinCommand_alert), ui);
  m.addCommand("prompt",     Command(builtinCommand_prompt), ui);
  m.addCommand("quit",       Command(builtinCommand_quit), ui);
  m.addCommand("movemaprel", Command(builtinCommand_moveMapRelative), ui);

  return m;
}

