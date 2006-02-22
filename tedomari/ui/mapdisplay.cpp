#include "ui/mapdisplay.h"

using namespace tedomari::ui;

void MapDisplay::paint()
{
  Region* r = getRegion();
  if (!game->isStarted()) {
    /* When game not started, we flood with green, as a proof of
     * concept. */
    r->flood(Colour::green);
    r->stroke(0, 0, r->getWidth(), r->getHeight(), Colour::red);
    //r->stroke(0, 0, 40, 40, red);
    //r->drawText("MapDisplay", red);
  } else {
    /* TODO: obviously we need a bit more functionality */
    //world->getUnits()
  }
}

