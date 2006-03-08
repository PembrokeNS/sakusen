#include "ui/builtincommand.h"

#include "stringutils.h"
#include "ui/ui.h"

using namespace std;

using namespace sakusen;

namespace tedomari {
namespace ui {

void tooFewArgs(
    const String& name,
    const list<String>& args,
    int expected, UI* ui
  )
{
  ostringstream o;
  o << "Too few arguments to command '" << name << "'.  Expected at least " <<
    expected << ", got " << args.size() << " ('" <<
    stringUtils_join(args, "' '") << "')";
  
  ui->alert(Alert(o.str()));
}

void tooManyArgs(const list<String>& args, int expected, UI* ui)
{
  ostringstream o;
  o << "Too many arguments to command.  Expected at most " << expected <<
    ", got " << args.size();
  
  ui->alert(Alert(o.str()));
}

void wrongNumberArgs(
    const String& name,
    const list<String>& args,
    int expected, UI* ui
  )
{
  ostringstream o;
  o << "Wrong number of arguments to command '" << name << "'.  Expected " <<
    expected << ", got " << args.size();
  
  ui->alert(Alert(o.str()));
}

void executeBuiltinCommand(
    BuiltinCommand cmd,
    const String& name,
    list<String>& args,
    UI* ui
  )
{
  switch (cmd) {
    case builtinCommand_alert:
      ui->alert(Alert(stringUtils_join(args, " ")));
      break;
    case builtinCommand_alias:
      {
        if (args.size() < 3) {
          tooFewArgs(name, args, 3, ui);
          break;
        }
        String mode = args.front();
        args.pop_front();
        String newCmd = args.front();
        args.pop_front();
        ui->addAlias(mode, newCmd, args);
      }
      break;
    case builtinCommand_bind:
      {
        if (args.size() < 3) {
          tooFewArgs(name, args, 3, ui);
          break;
        }
        String mode = args.front();
        args.pop_front();
        String keyName = args.front();
        args.pop_front();
        ui->addBinding(mode, keyName, args);
      }
      break;
    case builtinCommand_func:
      {
        if (args.size() < 4) {
          tooFewArgs(name, args, 3, ui);
          break;
        }
        String mode = args.front();
        args.pop_front();
        String functionName = args.front();
        args.pop_front();
        uint16 numArgs = numFromString<uint16>(args.front());
        args.pop_front();
        if (args.front() == "{") {
          args.pop_front();
          if (args.back() != "}") {
            ui->alert(Alert(
                  "In definition of function '" + functionName +
                  "', leading '{' was not matchied by a trailing '}'"
                ));
            break;
          }
          args.pop_back();
        }
        ui->addFunction(mode, Function(functionName, numArgs, args));
      }
      break;
    case builtinCommand_switchmode:
      {
        if (args.size() != 1) {
          wrongNumberArgs(name, args, 1, ui);
          break;
        }
        String mode = args.front();
        args.pop_front();
        ui->switchMode(mode);
      }
      break;
    case builtinCommand_prompt:
      if (!args.empty()) {
        tooManyArgs(args, 0, ui);
      }
      ui->setCommandEntry(true);
      break;
    case builtinCommand_quit:
      if (!args.empty()) {
        tooManyArgs(args, 0, ui);
      }
      ui->quitRequest();
      break;
    case builtinCommand_movemaprelfrac:
      {
        if (args.size() != 2) {
          wrongNumberArgs(name, args, 2, ui);
          break;
        }
        double dx = numFromString<double>(args.front());
        args.pop_front();
        double dy = numFromString<double>(args.front());
        ui->moveMapRelativeFrac(dx, dy);
      }
      break;
    case builtinCommand_startdragregion:
      if (!args.empty()) {
        tooManyArgs(args, 0, ui);
        break;
      }
      ui->dragRegion(true);
      break;
    case builtinCommand_stopdragregion:
      if (!args.empty()) {
        tooManyArgs(args, 0, ui);
        break;
      }
      ui->dragRegion(false);
      break;
    case builtinCommand_select:
      {
        if (args.size() != 1) {
          wrongNumberArgs(name, args, 1, ui);
          break;
        }
        ui->selectUnits(args.front());
      }
      break;
    default:
      Fatal("Unknown builtin command: " << cmd);
  }
}

}}

