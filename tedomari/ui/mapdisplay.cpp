#include "ui/mapdisplay.h"

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
    /* TODO: obviously we need a bit more functionality */
    r->setClip();
    r->fill(Colour::red);
    Map* m = sakusen::world->getMap();
    r->fillRect(dexToPixel(
          Rectangle<sint32>(m->left(), m->bottom(), m->width(), m->height())
        ), Colour::green);
    Rectangle<sint32> displayRect = pixelToDex(
        Rectangle<double>(0, 0, getWidth(), getHeight())
      );
    list<CompleteUnit*> unitsToDraw =
      sakusen::client::world->getUnitsIntersecting(displayRect);
    for (list<CompleteUnit*>::iterator unit = unitsToDraw.begin();
        unit != unitsToDraw.end(); ++unit) {
      list< Point<double> > corners;

      Point<uint32> corner = (*unit)->getSize();
      IUnitStatus* status = (*unit)->getStatus();
      corners.push_back(dexToPixel(status->localToGlobal(corner)));
      corner.x *= -1;
      corners.push_back(dexToPixel(status->localToGlobal(corner)));
      corner.y *= -1;
      corners.push_back(dexToPixel(status->localToGlobal(corner)));
      corner.x *= -1;
      corners.push_back(dexToPixel(status->localToGlobal(corner)));
      r->fillPolygon(corners, Colour::blue);
    }
    r->unsetClip();
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

