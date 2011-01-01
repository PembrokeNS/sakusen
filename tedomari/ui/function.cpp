#include "ui/function.h"

#include "ui/ui.h"

using namespace std;

using namespace sakusen;
using namespace tedomari::ui;

void Function::execute(const list<String>& argsList, UI* ui) const
{
  vector<String> args(argsList.begin(), argsList.end());
  assert(args.size() == numArgs);

  list<String> newBody;

  for (list<String>::const_iterator b = body.begin(); b != body.end(); ++b) {
    /* Substitute $i for argument i */
    if (!b->empty() && (*b)[0] == '$') {
      uint16 argNum = numFromString<uint16>(b->substr(1));
      if (argNum >= numArgs) {
        ui->alert(Alert(
              "argument number " + numToString(argNum) + " out of range"
            ));
      } else {
        newBody.push_back(args[argNum]);
      }
    } else {
      newBody.push_back(*b);
    }
  }

  ui->executeCommands(newBody);
}

