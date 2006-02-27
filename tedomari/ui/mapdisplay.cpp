#include "ui/mapdisplay.h"

using namespace sakusen;

using namespace tedomari::ui;

void MapDisplay::paint()
{
  Region* r = getRegion();
  if (!game->isStarted()) {
    /* When game not started, we flood with green, as a proof of
     * concept. */
    r->flood(Colour::green);
    r->stroke(0, 0, r->getWidth(), r->getHeight(), Colour::red);
  } else {
    /* TODO: obviously we need a bit more functionality */
    r->flood(Colour::red);
    Map* m = world->getMap();
    r->fillRect(dexToDisplay(
          Rectangle<sint32>(m->left(), m->bottom(), m->width(), m->height())
        ), Colour::blue);
    //world->getUnits()
  }
}

void MapDisplay::translate(const Point<sint32>& d)
{
  posOfDisplayZero = world->getMap()->addToPosition(posOfDisplayZero, d);
  getRegion()->paint();
  Debug("posOfDisplayZero=" << posOfDisplayZero);
}

void MapDisplay::translate(sint32 dx, sint32 dy)
{
  translate(Point<sint32>(dx, dy, 0));
}

