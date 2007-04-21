#ifndef TEDOMARI__UI__UIEXCEPTIONS_H
#define TEDOMARI__UI__UIEXCEPTIONS_H

#include "exceptions.h"

namespace tedomari {
namespace ui {

class ActionArgumentExn : sakusen::Exn {
  public:
    ActionArgumentExn(const String& message) : Exn(message) {}
};

class ActionExecuteExn : sakusen::Exn {
  public:
    ActionExecuteExn(const String& message) : Exn(message) {}
};

}}

#endif // TEDOMARI__UI__UIEXCEPTIONS_H

