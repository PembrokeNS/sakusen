#include "ui/sdl/cairo/cairolayout.h"
#include <pango/pangocairo.h>

using namespace tedomari::ui::sdl::cairo;

CairoLayout::CairoLayout(CairoUI* ui)
{
  layout = ui->newPangoLayout();
  /* May need to attach and detach the layout so that we can be informed
   * whenever the context changes - for the moment we don't for simplicity */
  //ui->attachLayout(this);
}

CairoLayout::~CairoLayout()
{
  //ui->detachLayout(this);
  g_object_unref(layout);
}

void CairoLayout::setText(const sakusen::String& t)
{
  pango_layout_set_text(layout, t.c_str(), -1);
}

uint16 CairoLayout::getHeight() const
{
  int w;
  int h;
  pango_layout_get_pixel_size(layout, &w, &h);
  return h;
}

void CairoLayout::contextChanged()
{
  pango_layout_context_changed(layout);
}
