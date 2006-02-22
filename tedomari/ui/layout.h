#ifndef UI__LAYOUT_H
#define UI__LAYOUT_H

#include "libsakusen-global.h"

#include "ui/colour.h"

namespace tedomari {
namespace ui {

class Layout {
  protected:
    Layout() : colour(Colour::white) {}
    Layout(const Layout& copy) : colour(copy.colour) {}
  private:
    Colour colour;
  public:
    virtual ~Layout() {}
    
    inline const Colour& getColour() const { return colour; }
    inline void setColour(const Colour& c) { colour = c; }
    virtual void setText(const String&) = 0;
    virtual uint16 getHeight() const = 0;

    virtual void contextChanged() = 0;
};

}}

#endif // UI__LAYOUT_H

