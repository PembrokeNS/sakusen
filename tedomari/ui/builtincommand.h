#ifndef TEDOMARI__UI__BUILTINCOMMAND_H
#define TEDOMARI__UI__BUILTINCOMMAND_H

#include "global.h"

#include <list>

namespace tedomari {
namespace ui {

class UI;

/** \brief Enumeration of commands built in the the GUI */
enum BuiltinCommand {
  builtinCommand_none,
  /* command-related commands */
  builtinCommand_alias,
  builtinCommand_bind,
  builtinCommand_func,
  /* general UI-related commands */
  builtinCommand_switchmode,
  builtinCommand_alert,
  builtinCommand_prompt,
  builtinCommand_regex,
  builtinCommand_quit,
  /* map-related commands */
  builtinCommand_movemaprelfrac,
  builtinCommand_startdragregion,
  builtinCommand_stopdragregion,
  /* unit-related commands */
  builtinCommand_select,
  /* action-related commands */
  builtinCommand_initializeaction,
  builtinCommand_abortaction,
  builtinCommand_supplyarg,
  builtinCommand_supplyregex
};

void executeBuiltinCommand(
    BuiltinCommand,
    const String& name,
    std::list<String>& args,
    UI* ui
  );

}}

#endif // TEDOMARI__UI__BUILTINCOMMAND_H

