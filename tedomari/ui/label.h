#ifndef TEDOMARI__UI__LABEL_H
#define TEDOMARI__UI__LABEL_H

#include "ui/control.h"

namespace tedomari {
namespace ui {

class Label : public Control {
  public:
    /** \brief Standard constructor
     *
     * Most arguments are the same as those for Control
     * \param t Initial text with which to fill label
     * \param foreCol Colour of text
     * \param backCol Colour of background
     */
    Label(
        uint16 x, uint16 y, DockStyle ds, Region* region,
        const String& t, const Colour& foreCol, const Colour& backCol
      ) :
      Control(x, y, ds, region),
      text(t),
      foreColour(foreCol),
      backColour(backCol)
    {}
  protected:
    String text;
    Colour foreColour;
    Colour backColour;

    void paint();
  public:
    void setText(const String& t) {
      text = t;
      paint();
    }
};

}}

#endif // TEDOMARI__UI__LABEL_H

