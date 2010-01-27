#ifndef TEDOMARI__UI__COMMANDENTRYBOX_H
#define TEDOMARI__UI__COMMANDENTRYBOX_H

#include "ui/textbox.h"

namespace tedomari {
namespace ui {

class UI;

/** \brief Represents the textbox which is used for input of commands in their
 * long form.
 */
class CommandEntryBox : public TextBox {
  public:
    /** \brief Standard constructor
     *
     * The arguments are the same as those for Control */
    CommandEntryBox(uint16 x, uint16 y, DockStyle ds, Region* region) :
      TextBox(x, y, ds, region, ":", Colour::black, Colour::white) {}
  private:
    void sendControlChar(uint16 c, UI* ui);
};

}}

#endif // TEDOMARI__UI__COMMANDENTRYBOX_H

