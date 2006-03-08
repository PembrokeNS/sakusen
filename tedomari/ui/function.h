#ifndef UI__FUNCTION_H
#define UI__FUNCTION_H

#include "libsakusen-global.h"

#include <list>

namespace tedomari {
namespace ui {

class UI;

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

#endif // FUNCTION_H

