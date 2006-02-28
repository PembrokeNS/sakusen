#include "sdlregion.h"

using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

void SDLRegion::setClip()
{
  ui->setClipRect(x, y, getWidth(), getHeight());
}

void SDLRegion::unsetClip()
{
  ui->resetClip();
}

void SDLRegion::fill(const Colour& c)
{
  ui->fillRect(x, y, getWidth(), getHeight(), c);
}

void SDLRegion::fillRect(
    double xoff,
    double yoff,
    double w,
    double h,
    const Colour& c
  )
{
  ui->fillRect(x+xoff, y+yoff, w, h, c);
}

void SDLRegion::fillPolygon(
    const std::list< sakusen::Point<double> >& poly,
    const Colour& c
  )
{
  ui->fillPolygon(poly, c);
}

void SDLRegion::stroke(
    double x1,
    double y1,
    double x2,
    double y2,
    const Colour& c
  )
{
  ui->stroke(x+x1, y+y1, x+x2, y+y2, c);
}

void SDLRegion::drawText(
    double xoff,
    double yoff,
    const String& text,
    const Colour& c
  )
{
  ui->drawText(x+xoff, y+yoff, text, c);
}

void SDLRegion::drawText(double xoff, double yoff, const Layout* layout)
{
  ui->drawText(x+xoff, y+yoff, dynamic_cast<const SDLLayout*>(layout));
}

