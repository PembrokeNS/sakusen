#include "ui/commandentrybox.h"

#include "stringutils.h"
#include "ui/ui.h"

using namespace sakusen;

using namespace tedomari::ui;

void CommandEntryBox::paint()
{
  if (visible) {
    /* Black background */
    getRegion()->fill(Colour::black);
    /* White text */
    getRegion()->drawText(0, 0, ":"+text, Colour::white);
  }
}

void CommandEntryBox::sendChar(uint16 c, UI* ui)
{
  if (c < ' ') {
    /* non-printable character */
    switch (c) {
      case '\b':
        if (text.empty()) {
          /** \todo Beep */
        } else {
          String::iterator newEnd =
            stringUtils_findPreviousCharStart(text.end(), text.begin());
          text.erase(newEnd, text.end());
        }
        break;
      case '\n':
      case '\r':
        ui->executeCommands(text);
        text = "";
        ui->setCommandEntry(false);
        return;
      case 27: /* ESC */
        text = "";
        ui->setCommandEntry(false);
        return;
      case '\t':
        /** \todo Tab completion */
        break;
      default:
        Debug("Unexpected unprintable character " << c);
        /** \todo Make a beep or something */
        break;
    }
  } else {
    text += stringUtils_charToUtf8(c);
  }
  getRegion()->paint();
}

