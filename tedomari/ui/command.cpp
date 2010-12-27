#include "ui/command.h"

#include "ui/alert.h"
#include "ui/ui.h"
#include <sakusen/heightfield-methods.h>

using namespace std;

namespace tedomari {
namespace ui {

void Command::execute(std::list<String>& args, UI* ui) const
{
  switch (type) {
    case commandType_ambiguous:
      ui->alert(Alert("Ambiguous command"));
      break;
    case commandType_abbreviation:
      ui->executeCommand(fullName, args);
      break;
    case commandType_builtin:
      executeBuiltinCommand(builtin, name, args, ui);
      break;
    case commandType_alias:
      {
        list<String> extraArgs = alias;
        String cmdName = extraArgs.front();
        extraArgs.pop_front();
        extraArgs.splice(extraArgs.end(), args);
        ui->executeCommand(cmdName, extraArgs);
      }
      break;
    case commandType_function:
      if (args.size() != function.getNumArgs()) {
        ui->alert(Alert(
              "Wrong number of arguments to function '" + function.getName() +
              "'"
            ));
        break;
      }
      function.execute(args, ui);
      break;
    default:
      SAKUSEN_FATAL("Unexpected CommandType: " << type);
  }
}

}}

