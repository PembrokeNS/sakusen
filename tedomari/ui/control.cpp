#include "ui/control.h"

#include "rectangle.h"

using namespace std;

using namespace sakusen;
using namespace tedomari::ui;

Control::Control(uint16 xoffset, uint16 yoffset, DockStyle ds, Region* r) :
  x(xoffset),
  y(yoffset),
  desiredHeight(r->getHeight()),
  desiredWidth(r->getWidth()),
  dockStyle(ds),
  region(r)
{
  subControls.push_back(list<Control*>());
}

Control::~Control()
{
  delete region;
  while (!subControls.empty()) {
    while (!subControls.front().empty()) {
      delete subControls.front().front();
      subControls.front().pop_front();
    }
    subControls.pop_front();
  }
}

void Control::replaceRegion(Region* newRegion)
{
  delete region;
  region = newRegion;

  for (list< list<Control*> >::iterator layer = subControls.begin();
      layer != subControls.end(); ++layer) {
    for (list<Control*>::iterator subControl = layer->begin();
        subControl != layer->end(); ++subControl) {
      uint16 x = min(getWidth(), (*subControl)->getX());
      uint16 y = min(getHeight(), (*subControl)->getY());
      uint16 w = min(uint16(getWidth() - x), (*subControl)->getWidth());
      uint16 h = min(uint16(getHeight() - y), (*subControl)->getHeight());
      (*subControl)->x = x;
      (*subControl)->y = y;
      (*subControl)->replaceRegion(region->newSubRegion(x, y, w, h));
    }
  }
}

/** \bug If I knew what this function were supposed to do, I could
 * check, but I suspect it is wrong. DH */
void Control::alignSubControls()
{
  for (list< list<Control*> >::iterator layer = subControls.begin();
      layer != subControls.end(); ++layer) {
    Rectangle<uint16> remainingArea(0, 0, getWidth(), getHeight());
    
    for (list<Control*>::iterator subControl = layer->begin();
        subControl != layer->end(); ++subControl) {
      Rectangle<uint16> controlArea(remainingArea);
      switch ((*subControl)->getDockStyle()) {
        case dockStyle_top:
          controlArea.maxy = controlArea.miny +
            min((*subControl)->desiredHeight, remainingArea.getHeight());
          remainingArea.miny = controlArea.maxy;
          break;
        case dockStyle_bottom:
          controlArea.miny = controlArea.maxy -
	    min((*subControl)->desiredHeight, remainingArea.getHeight());
          remainingArea.maxy = controlArea.miny;
          break;
        case dockStyle_fill:
          remainingArea.minx = remainingArea.maxx;
          break;
        default:
          Fatal("Unexpected DockStyle: " << (*subControl)->getDockStyle());
      }
      (*subControl)->x = remainingArea.getMinX();
      (*subControl)->y = remainingArea.getMinY();
      (*subControl)->replaceRegion(region->newSubRegion(controlArea));
      (*subControl)->alignSubControls();
    }
  }
}

void Control::paint()
{
  for (list< list<Control*> >::iterator layer = subControls.begin();
      layer != subControls.end(); ++layer) {
    for (list<Control*>::iterator subControl = layer->begin();
        subControl != layer->end(); ++subControl) {
      (*subControl)->paint();
    }
  }
}

