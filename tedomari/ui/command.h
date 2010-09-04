#ifndef TEDOMARI__UI__COMMAND_H
#define TEDOMARI__UI__COMMAND_H

#include <list>

#include <sakusen/libsakusen-global.h>
#include "ui/builtincommand.h"
#include "ui/function.h"

namespace tedomari {
namespace ui {

class UI;

/** \brief Enumeration of types of Command */
enum CommandType {
  /** A command that is the longest common prefix of two other
   * commands */
  commandType_ambiguous,
  /** A command that is an unambiguous prefix of another command */
  commandType_abbreviation,
  /** A command that ties directly to a built in command */
  commandType_builtin,
  /** A command that is an alias of another command */
  commandType_alias,
  /** A command that is linked to a user-defined function */
  commandType_function
};

/** \brief Represents a named command that can be entered at the GUI prompt
 *
 * This class represents all named commands or prefixes of named commands
 * defined in a given Mode.  Prefixes are stored also to aid in tab completion,
 * and also to allow automatic extrapolation of unambiguous prefixes.
 *
 * Note thtat commands are stored in a container, indexed by name, but the name
 * is not itself a field of the Command except when the Command is of a builtin
 * type. */
class Command {
  public:
    /** \brief Constructs an ambiguous command */
    Command() :
      type(commandType_ambiguous), fullName(), builtin(builtinCommand_none),
      name(), alias(), function()
    {}
    /** \brief Constructs an abbreviation command
     *
     * \param fn The full name of the command for which this is an abbreviation
     * */
    Command(const String& fn) :
      type(commandType_abbreviation), fullName(fn),
      builtin(builtinCommand_none), name(), alias(), function()
    {}
    /** \brief Constructs a builtin command
     *
     * \param c The builtin command which this Command executes
     * \param n The name of this Command */
    Command(BuiltinCommand c, const String& n) :
      type(commandType_builtin), fullName(), builtin(c), name(n), alias(),
      function()
    {}
    /** \brief Constructs an alias command
     *
     * \param a The (tokenized) command for which this is an alias.
     *
     * Arguments given to this command are appended to any arguments which are
     * already included as part of the alias. */
    Command(const std::list<String>& a) :
      type(commandType_alias), fullName(), builtin(builtinCommand_none),
      name(), alias(a), function()
    {
      assert(!a.empty());
    }
    /** \brief Constructs a function command
     *
     * \param f The Function which this command executes */
    Command(const Function& f) :
      type(commandType_function), fullName(), builtin(builtinCommand_none),
      name(), alias(), function(f)
    {}
  private:
    CommandType type;
    /* Only one or two of the following fields is ever used, so this is a bit
     * memory-wasteful. */
    String fullName; /* Only when abbreviation */
    BuiltinCommand builtin; /* Only when builtin */
    String name; /* Only when builtin */
    std::list<String> alias; /* Only when alias */
    Function function; /* Only when function */
  public:
    /** \brief Get the type of this Command (see CommandType) */
    CommandType getType() const { return type; }
    /** \brief Execute this command with the given arguments
     *
     * \warning This method modifies its argument args
     */
    void execute(std::list<String>& args, UI* ui) const;
};

}}

#endif // TEDOMARI__UI__COMMAND_H

