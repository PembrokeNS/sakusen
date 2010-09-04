#ifndef TEDOMARI__UI__FUNCTION_H
#define TEDOMARI__UI__FUNCTION_H

#include "global.h"

#include <list>

namespace tedomari {
namespace ui {

class UI;

/** \brief Represents a user-defined function
 *
 * This class represents a sequence of commands gathered in a function by a
 * user. */
class Function {
  public:
    Function() : name(), numArgs(0), body() {}
    Function(String n, uint16 nA, std::list<String> b) :
      name(n), numArgs(nA), body(b) {}
  private:
    String name;
    uint16 numArgs;
    std::list<String> body;
  public:
    const String& getName() const { return name; }
    uint16 getNumArgs() const { return numArgs; }

    void execute(const std::list<String>& args, UI* ui) const;
};

}}

#endif // TEDOMARI__UI__FUNCTION_H

