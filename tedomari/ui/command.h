#ifndef COMMAND_H
#define COMMAND_H

#include <list>

#include "libsakusen-global.h"
#include "ui/builtincommand.h"
#include "ui/function.h"

namespace tedomari {
namespace ui {

class UI;

enum CommandType {
  commandType_ambiguous,
  commandType_abbreviation,
  commandType_builtin,
  commandType_alias,
  commandType_function
};

class Command {
  public:
    Command() :
      type(commandType_ambiguous), fullName(), builtin(builtinCommand_none),
      name(), alias(), function()
    {}
    Command(const String& fn) :
      type(commandType_abbreviation), fullName(fn),
      builtin(builtinCommand_none), name(), alias(), function()
    {}
    Command(BuiltinCommand c, const String& n) :
      type(commandType_builtin), fullName(), builtin(c), name(n), alias(),
      function()
    {}
    Command(const std::list<String>& a) :
      type(commandType_alias), fullName(), builtin(builtinCommand_none),
      name(), alias(a), function()
    {}
    Command(const Function& f) :
      type(commandType_function), fullName(), builtin(builtinCommand_none),
      name(), alias(), function(f)
    {}
  private:
    CommandType type;
    /* Only one or two of the following fields is every used, so this is a bit
     * memory-wasteful. */
    String fullName; /* Only when abbreviation */
    BuiltinCommand builtin; /* Only when builtin */
    String name; /* Only when builtin */
    std::list<String> alias; /* Only when alias */
    Function function; /* Only when function */
  public:
    CommandType getType() const { return type; }
    /** \brief Execute this command with the given arguments
     *
     * \warning This method modifies its argument args
     */
    void execute(std::list<String>& args, UI* ui) const;
};

}}

#endif // COMMAND_H

