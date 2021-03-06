#include "ui/commandentrybox.h"

#include "ui/ui.h"

using namespace sakusen;

using namespace tedomari::ui;

void CommandEntryBox::sendControlChar(uint16 c, UI* ui)
{
  switch (c) {
    case '\n':
    case '\r':
      assert(!text.empty());
      /* remove leading ':' */
      text.erase(text.begin());
      ui->executeCommands(text);
      text = ":";
      ui->setCommandEntry(false);
      return;
    case 27: /* ESC */
      text = ":";
      ui->setCommandEntry(false);
      return;
    case '\t':
      /** \todo Tab completion */
      break;
    default:
      SAKUSEN_DEBUG("Unexpected unprintable character " << c);
      /** \todo Make a beep or something */
      break;
  }
}

