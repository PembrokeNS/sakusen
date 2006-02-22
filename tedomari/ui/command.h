#ifndef COMMAND_H
#define COMMAND_H

#include <list>

#include "libsakusen-global.h"
#include "builtincommand.h"

namespace tedomari {
namespace ui {

class UI;

enum CommandType {
  commandType_ambiguous,
  commandType_abbreviation,
  commandType_builtin,
  commandType_alias
};

class Command {
  public:
    Command() :
      type(commandType_ambiguous), fullName(), builtin(builtinCommand_none),
      alias()
    {}
    Command(const String& fn) :
      type(commandType_abbreviation), fullName(fn),
      builtin(builtinCommand_none), alias()
    {}
    Command(BuiltinCommand c) :
      type(commandType_builtin), fullName(), builtin(c), alias()
    {}
    Command(const std::list<String>& a) :
      type(commandType_alias), fullName(), builtin(builtinCommand_none),
      alias(a)
    {}
  private:
    CommandType type;
    /* Only one of the following fields is every used, so this is a bit
     * memory-wasteful. */
    String fullName; /* Only when abbreviation */
    BuiltinCommand builtin; /* Only when builtin */
    std::list<String> alias; /* Only when alias */
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

