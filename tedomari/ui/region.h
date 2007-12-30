#ifndef UI__REGION_H
#define UI__REGION_H

#include "libsakusen-global.h"

#include <list>

#include "rectangle.h"
#include "ui/colour.h"
#include "ui/layout.h"
#include "ui/surface.h"

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

    virtual void paint() const = 0;

    virtual Region* newSubRegion(
        uint16 x,
        uint16 y,
        uint16 width,
        uint16 height
      ) const = 0;
    inline Region* newSubRegion(const sakusen::Rectangle<uint16>& area) const {
      return newSubRegion(area.getMinX(), area.getMinY(), area.getWidth(), area.getHeight());
    }
    virtual sakusen::Point<double> globalToLocal(
        const sakusen::Point<double>&
      ) = 0;
    virtual sakusen::Point<double> localToGlobal(
        const sakusen::Point<double>&
      ) = 0;
    virtual Layout* newLayout() const = 0;

    /* Drawing functions */
    virtual void blit(const Surface::ConstPtr&) = 0;
    virtual void setClip() = 0;
    virtual void unsetClip() = 0;
    virtual void fill(const Colour&) = 0;
    virtual void fillRect(
        double x,
        double y,
        double w,
        double h,
        const Colour&
      ) = 0;
    virtual void fillCircle(double x, double y, double r, const Colour&) = 0;
    virtual void fillPolygon(
        const std::list< sakusen::Point<double> >&,
        const Colour&
      ) = 0;
    inline void fillRect(
        const sakusen::Rectangle<double>& rect,
        const Colour& c
      ) {
      fillRect(
          rect.getMinX(), rect.getMinY(), rect.getWidth(), rect.getHeight(), c
        );
    }
    inline void fillCircle(
        const sakusen::Point<double>& p,
        double r,
        const Colour& c
      ) {
      fillCircle(p.x, p.y, r, c);
    }
    virtual void stroke(
        double x1,
        double y1,
        double x2,
        double y2,
        const Colour&
      ) = 0;
    virtual void drawRect(
        double x,
        double y,
        double w,
        double h,
        const Colour& c
      ) = 0;
    inline void drawRect(const sakusen::Rectangle<double>& r, const Colour& c) {
      drawRect(r.getMinX(), r.getMinY(), r.getWidth(), r.getHeight(), c);
    }
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

