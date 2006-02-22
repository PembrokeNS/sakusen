#ifndef UI__BUILTINCOMMAND_H
#define UI__BUILTINCOMMAND_H

#include "libsakusen-global.h"

#include <list>

namespace tedomari {
namespace ui {

class UI;

enum BuiltinCommand {
  builtinCommand_none,
  builtinCommand_alert,
  builtinCommand_alias,
  builtinCommand_bind,
  builtinCommand_prompt,
  builtinCommand_quit
};

/* \brief Execute given buitin command with given arguments
 *
 * \warning Modifies its argument args */
void executeBuiltinCommand(
    BuiltinCommand,
    std::list<String>& args,
    UI* ui
  );

}}

#endif // UI__BUILTINCOMMAND_H

