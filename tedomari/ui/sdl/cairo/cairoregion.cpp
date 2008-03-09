#include "ui/sdl/cairo/cairoregion.h"

namespace tedomari {
namespace ui {
namespace sdl {
namespace cairo {

void CairoRegion::blit(const Surface::ConstPtr& s)
{
  ui->blit(x, y, boost::dynamic_pointer_cast<const SDLSurface>(s));
}

void CairoRegion::setClip()
{
  ui->setClipRect(x, y, getWidth(), getHeight());
}

void CairoRegion::unsetClip()
{
  ui->resetClip();
}

void CairoRegion::fill(const Colour& c)
{
  ui->fillRect(x, y, getWidth(), getHeight(), c);
}

void CairoRegion::fillRect(
    double xoff,
    double yoff,
    double w,
    double h,
    const Colour& c
  )
{
  ui->fillRect(x+xoff, y+yoff, w, h, c);
}

void CairoRegion::fillCircle(
    double xoff,
    double yoff,
    double r,
    const Colour& c
  )
{
  ui->fillCircle(x+xoff, y+yoff, r, c);
}

void CairoRegion::fillPolygon(
    const std::list< sakusen::Point<double> >& poly,
    const Colour& c
  )
{
  ui->fillPolygon(poly, c);
}

void CairoRegion::stroke(
    double x1,
    double y1,
    double x2,
    double y2,
    const Colour& c
  )
{
  ui->stroke(x+x1, y+y1, x+x2, y+y2, c);
}

void CairoRegion::drawRect(
    double xoff,
    double yoff,
    double w,
    double h,
    const Colour& c
  )
{
  ui->drawRect(x+xoff, y+yoff, w, h, c);
}

void CairoRegion::drawText(
    double xoff,
    double yoff,
    const String& text,
    const Colour& c
  )
{
  ui->drawText(x+xoff, y+yoff, text, c);
}

void CairoRegion::drawText(double xoff, double yoff, const Layout* layout)
{
  ui->drawText(x+xoff, y+yoff, dynamic_cast<const CairoLayout*>(layout));
}

}}}}


