#ifndef UI_SDL_CAIRO__CAIROUI_H
#define UI_SDL_CAIRO__CAIROUI_H

#include "ui/sdl/sdlui.h"

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;
typedef struct _PangoLayout PangoLayout;
typedef struct _PangoContext PangoContext;

namespace tedomari {
namespace ui {
namespace sdl {
namespace cairo {

class CairoLayout;

class CairoUI : public SDLUI {
  private:
    CairoUI();
    CairoUI(const CairoUI&);
  public:
    CairoUI(
        const SDLUI::Options&,
        std::ifstream& uiconf,
        tedomari::game::Game*
      );
    ~CairoUI();
  protected:
    cairo_surface_t* cairoBuffer; /**< cairo's wrapper of buffer */
    cairo_t* cairoContext; /**< Context for cairoBuffer */
    PangoContext* pangoContext;

    void resize(uint16 width, uint16 height);
    Region* newRegion(uint16 x, uint16 y, uint16 width, uint16 height);
  public:
    PangoLayout* newPangoLayout();
    void cairoSetSource(const Colour& c);
    void setClipRect(double x, double y, double w, double h);
    void resetClip();
    void fillRect(double x, double y, double w, double h, const Colour&);
    void fillPolygon(
        const std::list< sakusen::Point<double> >& poly,
        const Colour&
      );
    void stroke(double x1, double y1, double x2, double y2, const Colour&);
    void drawRect(double x, double y, double w, double h, const Colour& c);
    void drawText(double x, double y, const String& text, const Colour&);
    void drawText(double x, double y, const CairoLayout* layout);
};

}}}}

#endif // UI_SDL_CAIRO__CAIROUI_H

