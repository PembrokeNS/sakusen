#ifndef COMMANDENTRYBOX_H
#define COMMANDENTRYBOX_H

#include "ui/control.h"

namespace tedomari {
namespace ui {

class UI;

/* TODO: If there ever exist more text boxes then this class should inherit
 * from a TextBox class, and the code be divided appropriately */
class CommandEntryBox : public Control {
  public:
    CommandEntryBox(uint16 x, uint16 y, DockStyle ds, Region* region) :
    Control(x, y, ds, region) {}
  private:
    bool visible;
    String text;
  protected:
    void paint();
  public:
    inline void setVisible(bool v) { visible = v; }
    void sendChar(uint16 c, UI* ui);
};

}}

#endif // COMMANDENTRYBOX_H

