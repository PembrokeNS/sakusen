#ifndef COMMANDENTRYBOX_H
#define COMMANDENTRYBOX_H

#include "ui/control.h"

namespace tedomari {
namespace ui {

class UI;

/** \brief Represents the textbox which is used for input of commands in their
 * long form.
 *
 * \todo If there ever exist more text boxes then this class should inherit
 * from a TextBox class, and the code be divided appropriately */
class CommandEntryBox : public Control {
  public:
    /** \brief Standard constructor
     *
     * The arguments are the same as those for Control */
    CommandEntryBox(uint16 x, uint16 y, DockStyle ds, Region* region) :
    Control(x, y, ds, region) {}
  private:
    bool visible;
    String text;
  protected:
    void paint();
  public:
    /** \brief Set whether the command entry box is visible */
    inline void setVisible(bool v) { visible = v; }
    void sendChar(uint16 c, UI* ui);
};

}}

#endif // COMMANDENTRYBOX_H

