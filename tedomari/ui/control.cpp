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

/** \brief Aligns the subcontrols of this control according to their deisred
 * widths, heights, etc.
 *
 * Each layer of subcontrols is one element of the subControls list, and is
 * itself a list of controls.  Each layer is allocated the entire area of this
 * control to distribute its controls amongst.  Space is allocated on a
 * first-com-first-serverd basis, working through the list, according to each
 * subcontrol's properties.  The pertinent properties are the DockStyle, which
 * describes which edge of the control the subcontrol is attached to (or, in
 * the case of dockStyle_fill, that the control wishes to take all the
 * remaining are) and desired width and height.
 *
 * The controls is the later layers will overlap the
 * controls in the earlier layers so they should either not cover the whole
 * area or be transparent if the earlier layers are to be at all visible. */
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

