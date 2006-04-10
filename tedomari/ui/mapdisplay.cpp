#include "ui/mapdisplay.h"

#include "ui/ui.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::client;

using namespace tedomari::ui;

void MapDisplay::paint()
{
  Region* r = getRegion();
  if (!game->isStarted()) {
    /* When game not started, we flood with green, as a proof of
     * concept. */
    r->fill(Colour::green);
    r->stroke(0, 0, r->getWidth(), r->getHeight(), Colour::red);
  } else {
    /** \todo obviously we need a bit more functionality */
    r->setClip();
    r->fill(Colour::red);
    Map* m = sakusen::world->getMap();
    r->fillRect(dexToPixel(
          Rectangle<sint32>(m->left(), m->bottom(), m->right(), m->top())
        ), Colour::green);
    Rectangle<sint32> displayRect = pixelToDex(
        Rectangle<double>(0, 0, getWidth(), getHeight())
      );
    list<UpdatedUnit*> unitsToDraw =
      sakusen::client::world->getUnitsIntersecting(displayRect);
    for (list<UpdatedUnit*>::iterator unit = unitsToDraw.begin();
        unit != unitsToDraw.end(); ++unit) {
      list< Point<double> > corners;

      Point<uint32> corner = (*unit)->getTypeData().getSize();
      const UnitStatus& status = (*unit)->getStatus();
      corners.push_back(dexToPixel(status.localToGlobal(corner)));
      corner.x *= -1;
      corners.push_back(dexToPixel(status.localToGlobal(corner)));
      corner.y *= -1;
      corners.push_back(dexToPixel(status.localToGlobal(corner)));
      corner.x *= -1;
      corners.push_back(dexToPixel(status.localToGlobal(corner)));
      bool selected = ui->getSelection().count((*unit)->getId());
      Colour colour = ( selected ? Colour::magenta : Colour::blue );
      r->fillPolygon(corners, colour);
    }
    
    if (dragging) {
      Debug("Drawing the selection rectangle");
      /* Draw the selection rectangle as it stands */
      Rectangle<double> selection(dexToPixel(dragStart), mousePos);
      r->drawRect(selection, Colour::grey(0.5));
    }
    
    r->unsetClip();
  }
}

void MapDisplay::update()
{
  if (game->isDirty()) {
    game->clearDirty();
    getRegion()->paint();
  }
}

void MapDisplay::mouseMove(const sakusen::Point<double>& p)
{
  mousePos = p;
  if (dragging) {
    getRegion()->paint();
  }
}

void MapDisplay::translate(const Point<sint32>& d)
{
  posOfDisplayZero =
    sakusen::world->getMap()->addToPosition(posOfDisplayZero, d);
  getRegion()->paint();
  Debug("posOfDisplayZero=" << posOfDisplayZero);
}

void MapDisplay::translate(sint32 dx, sint32 dy)
{
  translate(Point<sint32>(dx, dy, 0));
}

void MapDisplay::startDrag()
{
  dragging = true;
  dragStart = pixelToDex(mousePos);
}

Rectangle<sint32> MapDisplay::stopDrag()
{
  if (!dragging) {
    Debug("stopped dragging when wasn't dragging");
    return Rectangle<sint32>();
  }

  dragging = false;
  return Rectangle<sint32>(dragStart, pixelToDex(mousePos));
}

