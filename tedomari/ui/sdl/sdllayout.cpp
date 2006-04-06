#include "ui/sdl/sdllayout.h"
#ifndef DISABLE_PANGO
#include <pango/pangocairo.h>


using namespace tedomari::ui::sdl;

SDLLayout::SDLLayout(SDLUI* ui)
{
  layout = ui->newPangoLayout();
  /* May need to attach and detach the layout so that we can be informed
   * whenever the context changes - for the moment we don't for simplicity */
  //ui->attachLayout(this);
}

SDLLayout::~SDLLayout()
{
  //ui->detachLayout(this);
  g_object_unref(layout);
}

void SDLLayout::setText(const String& t)
{
  pango_layout_set_text(layout, t.c_str(), -1);
}

uint16 SDLLayout::getHeight() const
{
  int w;
  int h;
  pango_layout_get_pixel_size(layout, &w, &h);
  return h;
}

void SDLLayout::contextChanged()
{
  pango_layout_context_changed(layout);
}

#else

using namespace tedomari::ui::sdl;

SDLLayout::SDLLayout(SDLUI* ui)
{
 /* May need to attach and detach the layout so that we can be informed
   * whenever the context changes - for the moment we don't for simplicity */
  //ui->attachLayout(this);
}

SDLLayout::~SDLLayout()
{
  //ui->detachLayout(this);
 }

void SDLLayout::setText(const String& t)
{
 }

uint16 SDLLayout::getHeight() const
{
  return 0;
 }

void SDLLayout::contextChanged()
{
 
}

#endif