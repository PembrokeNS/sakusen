#ifndef UI__BUILTINCOMMAND_H
#define UI__BUILTINCOMMAND_H

#include "libsakusen-global.h"

#include <list>

namespace tedomari {
namespace ui {

class UI;

enum BuiltinCommand {
  builtinCommand_none,
  /* command-related commands */
  builtinCommand_alias,
  builtinCommand_bind,
  /* general UI-related commands */
  builtinCommand_alert,
  builtinCommand_prompt,
  builtinCommand_quit,
  /* map-related commands */
  builtinCommand_moveMapRelative
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

