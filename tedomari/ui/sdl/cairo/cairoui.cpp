#include "tedomari-global.h"
#include "ui/sdl/cairo/cairoui.h"
#include "ui/sdl/cairo/cairoregion.h"

#include <pango/pangocairo.h>

using namespace std;
using namespace tedomari::game;
using namespace tedomari::ui;
using namespace tedomari::ui::sdl::cairo;

CairoUI::CairoUI(const SDLUI::Options& options, ifstream& uiConf, Game* game) :
  SDLUI(options, uiConf, game)
{
  /* Create the cairo interface to that buffer */
  cairoBuffer = cairo_image_surface_create_for_data(
      buffer, CAIRO_FORMAT_ARGB32, getWidth(), getHeight(), stride
    );
  cairoContext = cairo_create(cairoBuffer);
  pangoContext = pango_cairo_font_map_create_context(
      reinterpret_cast<PangoCairoFontMap*>(pango_cairo_font_map_get_default())
    );
  pango_cairo_update_context(cairoContext, pangoContext);
  /* Provide access to these buffers to the UI and associated controls */
  replaceRegion(new CairoRegion(this, 0, 0, getWidth(), getHeight()));
  /* Place the initial control set */
  initializeControls();
  /* Paint the controls onto the buffer */
  paint();
}

CairoUI::~CairoUI()
{
  g_object_unref(pangoContext);
  cairo_destroy(cairoContext);
  cairo_surface_destroy(cairoBuffer);
}

void CairoUI::resize(uint16 width, uint16 height)
{
  /* Call down to base class to reset SDL-related things */
  SDLUI::resize(width, height);
  /* Create the new cairo interface to the buffer */
  cairo_destroy(cairoContext);
  cairo_surface_destroy(cairoBuffer);
  cairoBuffer = cairo_image_surface_create_for_data(
      buffer, CAIRO_FORMAT_ARGB32, width, height, stride
    );
  cairoContext = cairo_create(cairoBuffer);
  /* The following call entails a call to pango_layout_context_changed() for
   * all layouts.  At present this is arranged for by overloading replaceRegion
   * on those controls which have such a layout.  There might well be a better
   * way. */
  pango_cairo_update_context(cairoContext, pangoContext);
  /* Replace the region of the UI, and trigger the cascade of replacements and
   * a repainting of the controls */
  UI::resize(new CairoRegion(this, 0, 0, width, height));
}

Region* CairoUI::newRegion(uint16 x, uint16 y, uint16 width, uint16 height)
{
  assert(x+width <= getWidth());
  assert(y+height <= getHeight());
  return new CairoRegion(this, x, y, width, height);
}

PangoLayout* CairoUI::newPangoLayout()
{
  return pango_layout_new(pangoContext);
}

void CairoUI::cairoSetSource(const Colour& c)
{
  cairo_set_source_rgba(cairoContext, c.dr(), c.dg(), c.db(), c.da());
}

void CairoUI::setClipRect(double x, double y, double w, double h) {
  cairo_rectangle(cairoContext, x, y, w, h);
  cairo_clip(cairoContext);
}

void CairoUI::resetClip() {
  cairo_reset_clip(cairoContext);
}

void CairoUI::fillRect(double x, double y, double w, double h, const Colour& c)
{
  SDLDebug(
      "x=" << x << ", y=" << y << ", w=" << w << ", h=" << h << ", c.ia=" <<
      uint32(c.ia())
    );
  cairoSetSource(c);
  cairo_rectangle(cairoContext, x, y, w, h);
  cairo_fill(cairoContext);
}

void CairoUI::fillCircle(double x, double y, double r, const Colour& c)
{
  cairoSetSource(c);
  cairo_arc(cairoContext, x, y, r, 0, 2*M_PI);
  cairo_fill(cairoContext);
}

void CairoUI::fillPolygon(
    const list< sakusen::Point<double> >& poly,
    const Colour& c
  )
{
  SDLDebug(c.ir());
  assert(!poly.empty());

  list< sakusen::Point<double> >::const_iterator vertex = poly.begin();
  cairoSetSource(c);
  cairo_move_to(cairoContext, vertex->x, vertex->y);
  while (++vertex != poly.end()) {
    cairo_line_to(cairoContext, vertex->x, vertex->y);
  }
  cairo_close_path(cairoContext);
  cairo_fill(cairoContext);
}

void CairoUI::stroke(double x1, double y1, double x2, double y2, const Colour& c)
{
  SDLDebug("x1=" << x1 << ", y1=" << y1 << ", x2=" << x2 << ", y2=" << y2);
  cairoSetSource(c);
  cairo_move_to(cairoContext, x1, y1);
  cairo_line_to(cairoContext, x2, y2);
  cairo_stroke(cairoContext);
}

void CairoUI::drawRect(double x, double y, double w, double h, const Colour& c)
{
  SDLDebug("x=" << x << ", y=" << y << ", w=" << w << ", h=" << h);
  cairoSetSource(c);
  cairo_rectangle(cairoContext, x, y, w, h);
  cairo_stroke(cairoContext);
}

void CairoUI::drawText(double x, double y, const String& text, const Colour& c)
{
  cairoSetSource(c);
  PangoLayout* layout = pango_cairo_create_layout(cairoContext);

  /* Write some text */
  pango_layout_set_width(layout, -1);
  /* Note: This function takes a UTF-8 string, as we wish */
  pango_layout_set_text(layout, text.c_str(), -1);
  cairo_move_to(cairoContext, x, y);
  pango_cairo_show_layout(cairoContext, layout);

  /* Cleanup */
  g_object_unref(layout);
}

void CairoUI::drawText(double x, double y, const CairoLayout* sdlLayout)
{
  cairoSetSource(sdlLayout->getColour());

  /* Write some text */
  cairo_move_to(cairoContext, x, y);
  pango_cairo_show_layout(cairoContext, sdlLayout->getLayout());
}
