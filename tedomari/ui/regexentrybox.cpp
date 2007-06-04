#include "ui/commandentrybox.h"

#include "stringutils.h"
#include "ui/ui.h"

using namespace sakusen;

using namespace tedomari::ui;

void RegexEntryBox::sendControlChar(uint16 c, UI* ui)
{
  switch (c) {
    case '\n':
    case '\r':
      assert(!text.empty());
      /* remove leading '/' */
      text.erase(text.begin());
      ui->executeRegex(text);
      text = "/";
      ui->setRegexEntry(false);
      return;
    case 27: /* ESC */
      text = "/";
      ui->setRegexEntry(false);
      return;
    case '\t':
      /** \todo Tab completion */
      break;
    default:
      Debug("Unexpected unprintable character " << c);
      /** \todo Make a beep or something */
      break;
  }
}
