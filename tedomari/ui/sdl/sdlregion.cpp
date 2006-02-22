#include "sdlregion.h"

using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

void SDLRegion::flood(Colour c)
{
  ui->fillRect(x, y, getWidth(), getHeight(), c);
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

