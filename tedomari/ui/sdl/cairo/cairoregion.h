#ifndef UI_SDL_CAIRO__CAIROREGION_H
#define UI_SDL_CAIRO__CAIROREGION_H

#include "ui/region.h"
#include "ui/sdl/cairo/cairoui.h"
#include "ui/sdl/cairo/cairolayout.h"

namespace tedomari {
namespace ui {
namespace sdl {
namespace cairo {

class CairoRegion : public Region {
  private:
    CairoRegion();
    CairoRegion(const CairoRegion&);
  public:
    CairoRegion(
        CairoUI* u,
        uint16 xoffset,
        uint16 yoffset,
        uint16 width,
        uint16 height
      ) :
      Region(width, height), ui(u), x(xoffset), y(yoffset)
    {}
  private:
    CairoUI* ui; /* Not owned by this */
    uint16 x;
    uint16 y;
  public:
    inline void paint() const {
      /** \bug Should only repaint this region, not whole thing */
      ui->paint();
    }
    inline Region* newSubRegion(uint16 xoff, uint16 yoff, uint16 w, uint16 h)
      const {
      assert(xoff+w <= getWidth());
      assert(yoff+h <= getHeight());
      return new CairoRegion(ui, x+xoff, y+yoff, w, h);
    }
    inline sakusen::Point<double> globalToLocal(
        const sakusen::Point<double>& p
      ) {
      return p - sakusen::Point<double>(x, y, 0);
    }
    inline sakusen::Point<double> localToGlobal(
        const sakusen::Point<double>& p
      ) {
      return p + sakusen::Point<double>(x, y, 0);
    }
    inline Layout* newLayout() const { return new CairoLayout(ui); }
    void setClip();
    void unsetClip();
    void fill(const Colour&);
    void fillRect(double x, double y, double w, double h, const Colour&);
    void fillPolygon(
        const std::list< sakusen::Point<double> >&,
        const Colour&
      );
    void stroke(double x1, double y1, double x2, double y2, const Colour&);
    void drawRect(double x, double y, double w, double h, const Colour& c);
    void drawText(double x, double y, const String& text, const Colour&);
    void drawText(double x, double y, const Layout* layout);
};

}}}}

#endif // UI_SDL_CAIRO__CAIROREGION_H

