#ifndef UI__ALERTDISPLAY_H
#define UI__ALERTDISPLAY_H

#include "ui/layout.h"
#include "ui/control.h"
#include "ui/alert.h"

namespace tedomari {
namespace ui {

class AlertDisplay : public Control {
  private:
    AlertDisplay();
    AlertDisplay(const AlertDisplay&);
  public:
    AlertDisplay(uint16 x, uint16 y, DockStyle ds, Region* r) :
      Control(x, y, ds, r), nextSlot(1), lastOnDisplay(1),
      layout(r->newLayout())
    {}
    ~AlertDisplay() {
      delete layout;
    }
  private:
    Alert alerts[9];
    int nextSlot;
    int lastOnDisplay;
    Layout* layout; /* Owned by this */

    void refreshText();
    void replaceRegion(Region* region);
  public:
    void add(const Alert&);
    void paint();
};

}}

#endif // UI__ALERTDISPLAY_H

