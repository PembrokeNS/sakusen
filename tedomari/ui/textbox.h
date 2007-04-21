#ifndef UI__TEXBOX_H
#define UI__TEXBOX_H

#include "ui/label.h"

namespace tedomari {
namespace ui {

class UI;

class TextBox : public Label {
  public:
    /** \brief Standard constructor
     *
     * The arguments are the same as those for Label */
    TextBox(
        uint16 x, uint16 y, DockStyle ds, Region* region,
        const String& t, const Colour& foreCol, const Colour& backCol
      ) :
      Label(x, y, ds, region, t, foreCol, backCol)
    {}
  private:
    bool visible;
  protected:
    void paint();
    virtual void sendControlChar(uint16 c, UI* ui) = 0;
  public:
    /** \brief Set whether the command entry box is visible */
    inline void setVisible(bool v) { visible = v; }
    void sendChar(uint16 c, UI* ui);
};

}}

#endif // UI__TEXBOX_H

