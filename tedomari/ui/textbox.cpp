#include "ui/textbox.h"

#include <sakusen/stringutils.h>

using namespace sakusen;

using namespace tedomari::ui;

void TextBox::paint()
{
  if (visible) {
    Label::paint();
  }
}

void TextBox::sendChar(uint16 c, UI* ui)
{
  if (c < ' ') {
    /* non-printable character */
    switch (c) {
      case '\b':
        if (text.size() == 1) {
          /** \todo Beep */
        } else {
          String::iterator newEnd =
            stringUtils_findPreviousCharStart(text.end(), text.begin());
          text.erase(newEnd, text.end());
        }
        break;
      default:
        sendControlChar(c, ui);
        break;
    }
  } else {
    text += stringUtils_charToUtf8(c);
  }
  getRegion()->paint();
}

