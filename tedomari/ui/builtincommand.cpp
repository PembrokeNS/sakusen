#include "ui/builtincommand.h"

#include "stringutils.h"
#include "ui/ui.h"

using namespace std;

using namespace sakusen;

namespace tedomari {
namespace ui {

void tooFewArgs(const list<String>& args, int expected, UI* ui)
{
  ostringstream o;
  o << "Too few arguments to command.  Expected at least " << expected <<
    ", got " << args.size();
  
  ui->alert(Alert(o.str()));
}

void tooManyArgs(const list<String>& args, int expected, UI* ui)
{
  ostringstream o;
  o << "Too many arguments to command.  Expected at most " << expected <<
    ", got " << args.size();
  
  ui->alert(Alert(o.str()));
}

void wrongNumberArgs(const list<String>& args, int expected, UI* ui)
{
  ostringstream o;
  o << "Wrong number of arguments to command.  Expected " << expected <<
    ", got " << args.size();
  
  ui->alert(Alert(o.str()));
}

void executeBuiltinCommand(
    BuiltinCommand cmd,
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
          tooFewArgs(args, 3, ui);
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
          tooFewArgs(args, 3, ui);
          break;
        }
        String mode = args.front();
        args.pop_front();
        String keyName = args.front();
        args.pop_front();
        ui->addBinding(mode, keyName, args);
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
    case builtinCommand_moveMapRelative:
      {
        if (args.size() != 2) {
          wrongNumberArgs(args, 2, ui);
          break;
        }
        double dx = numFromString<double>(args.front());
        args.pop_front();
        double dy = numFromString<double>(args.front());
        ui->moveMapRelativeFrac(dx, dy);
      }
      break;
    default:
      Fatal("Unknown builtin command: " << cmd);
  }
}

}}

