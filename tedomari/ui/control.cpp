#include "ui/control.h"

#include <sakusen/rectangle.h>

using namespace std;

using namespace sakusen;
using namespace tedomari::ui;

/** \brief Standard constructor
 *
 * \param xoffset Preffered x coordinate of Control (At present this has little
 * or no effect because of docking &mdash; see DockStyle).
 * \param yoffset Preffered y coordinate of Control (Also has little or no
 * effect).
 * \param ds DockStyle to use when aligning this control within its parent
 * control.
 * \param r Region that this control initially occupies */
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

/** \brief Destructor
 *
 * The destructor destroys all subcontrols.  Further behaviour is not excluded.
 * */
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

/** \brief Replace the region occupied by this control
 *
 * This method also recursively replaces regions of all subcontrols.
 *
 * This method will be called for all controls when the underlying surface used
 * for rendering is replaced (e.g. because of a change in video mode).  So,
 * subclasses can override this method to perform actions which are necessary
 * at such times, but they must call this method as part of the overriding
 * method. */
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
          SAKUSEN_FATAL("Unexpected DockStyle: " << (*subControl)->getDockStyle());
      }
      (*subControl)->x = remainingArea.getMinX();
      (*subControl)->y = remainingArea.getMinY();
      (*subControl)->replaceRegion(region->newSubRegion(controlArea));
      (*subControl)->alignSubControls();
    }
  }
}

/** \brief Paint this control to the UI
 *
 * The base version of this method simply recurses to call paint for all
 * subcontols.  Subclasses should override this method to perform their own
 * painting. */
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

