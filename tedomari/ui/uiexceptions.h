#ifndef TEDOMARI__UI__UIEXCEPTIONS_H
#define TEDOMARI__UI__UIEXCEPTIONS_H

#include <sakusen/exceptions.h>

namespace tedomari {
namespace ui {

class ActionArgumentExn : public sakusen::Exn {
  public:
    ActionArgumentExn(const String& message) : Exn(message) {}
};

class ActionExecuteExn : public sakusen::Exn {
  public:
    ActionExecuteExn(const String& message) : Exn(message) {}
};

}}

#endif // TEDOMARI__UI__UIEXCEPTIONS_H

