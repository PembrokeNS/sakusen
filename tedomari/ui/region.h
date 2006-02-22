#ifndef UI__REGION_H
#define UI__REGION_H

#include "libsakusen-global.h"

#include "ui/colour.h"
#include "ui/rectangle.h"
#include "ui/layout.h"

namespace tedomari {
namespace ui {

class Region {
  private:
    Region();
    Region(const Region&);
  public:
    Region(uint16 w, uint16 h) : width(w), height(h) {}
    virtual ~Region() {}
  private:
    uint16 width;
    uint16 height;
  public:
    inline uint16 getWidth() const { return width; }
    inline uint16 getHeight() const { return height; }

    virtual Region* newSubRegion(
        uint16 x,
        uint16 y,
        uint16 width,
        uint16 height
      ) = 0;
    inline Region* newSubRegion(const Rectangle<uint16>& area) {
      return newSubRegion(area.x, area.y, area.width, area.height);
    }
    virtual Layout* newLayout() = 0;

    /* Drawing functions */
    virtual void flood(Colour) = 0;
    virtual void stroke(
        double x1,
        double y1,
        double x2,
        double y2,
        const Colour&
      ) = 0;
    virtual void drawText(
        double x,
        double y,
        const String& text,
        const Colour&
      ) = 0;
    virtual void drawText(double x, double y, const Layout* layout) = 0;
};

}}

#endif // UI__REGION_H

