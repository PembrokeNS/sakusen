#ifndef TEDOMARI__UI__ALERTDISPLAY_H
#define TEDOMARI__UI__ALERTDISPLAY_H

#include <boost/scoped_ptr.hpp>

#include "ui/layout.h"
#include "ui/control.h"
#include "ui/alert.h"

namespace tedomari {
namespace ui {

class AlertDisplay : public Control {
  private:
    /** \name Private constructors
     *
     * These constructors should not be used */
    //@{
    AlertDisplay();
    AlertDisplay(const AlertDisplay&);
    //@}
  public:
    /** \brief Standard constructor.
     *
     * Arguments are the same as those for Control. */
    AlertDisplay(uint16 x, uint16 y, DockStyle ds, Region* r) :
      Control(x, y, ds, r), nextSlot(1), lastOnDisplay(1),
      layout(r->newLayout())
    {}
  private:
    Alert alerts[9];
    int nextSlot;
    int lastOnDisplay;
    boost::scoped_ptr<Layout> layout; /* Owned by this */

    void refreshText();
    void replaceRegion(Region* region);
  public:
    void add(const Alert&);
    void paint();
};

}}

#endif // TEDOMARI__UI__ALERTDISPLAY_H

