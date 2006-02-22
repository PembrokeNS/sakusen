#include "ui/command.h"

#include "ui/alert.h"
#include "ui/ui.h"

using namespace std;

using namespace tedomari::ui;

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
      executeBuiltinCommand(builtin, args, ui);
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
    default:
      Fatal("Unexpected CommandType: " << type);
  }
}

