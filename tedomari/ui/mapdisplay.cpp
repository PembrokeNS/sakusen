#include "ui/mapdisplay.h"

#include "ui/ui.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::client;

using namespace tedomari::ui;

/** \brief Draw a unit.
 * \param[in] unit The unit to draw. Must not be NULL.
 * \param[in] colour The colour of the player who owns this unit.
 */
template<typename UnitRef>
void MapDisplay::drawUnit(
    const UnitRef& unit,
    const Colour& colour
  )
{
  list< Point<double> > corners;

  Point<uint32> corner = unit->getITypeData()->getSize();
  const UnitStatus& status = unit->getIStatus();
  corners.push_back(dexToPixel(status.localToGlobal(corner)));
  corner.x *= -1;
  corners.push_back(dexToPixel(status.localToGlobal(corner)));
  corner.y *= -1;
  corners.push_back(dexToPixel(status.localToGlobal(corner)));
  corner.x *= -1;
  corners.push_back(dexToPixel(status.localToGlobal(corner)));
  getRegion()->fillPolygon(corners, colour);
}

/** \brief Draw a graphical representation of a unit's orders.
 * \param[in] unit The unit to draw. Must not be NULL.
 * \param[in] colour The colour of the player who owns this unit.
 */
void MapDisplay::drawUnitOrders(
    const Ref<const UpdatedUnit>& unit,
    const Colour& colour
  )
{
  Point<double> targetPosition;
  Point<double> currentPosition;
  UnitOrders uo = unit->getUnitOrders();

  if (uo.getLinearTarget() == linearTargetType_position) {
    targetPosition = dexToPixel(uo.getTargetPosition());
    currentPosition = dexToPixel(unit->getIStatus()->getPosition());
    getRegion()->stroke(currentPosition.x, currentPosition.y,
        targetPosition.x, targetPosition.y, colour);
  }
}

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
    
    ISpatial::Result unitsToDraw =
      sakusen::client::world->getSpatialIndex()->findIntersecting(
          displayRect, gameObject_unit
        );
    /*QDebug("drawing " << unitsToDraw.size() << " units");*/
    for (ISpatial::Result::iterator unitIt = unitsToDraw.begin();
        unitIt != unitsToDraw.end(); ++unitIt) {
      Ref<UpdatedUnit> unit = unitIt->dynamicCast<UpdatedUnit>();
      bool selected = ((ui->getSelection().count(unit->getId()))!=0);
      Colour colour = ( selected ? Colour::magenta : Colour::blue );
      drawUnit(unit, colour);
      drawUnitOrders(unit, colour);
    }
    
    ISpatial::Result returnsToDraw =
      sakusen::client::world->getSpatialIndex()->findIntersecting(
          displayRect, gameObject_sensorReturns
        );
    /*QDebug("drawing " << unitsToDraw.size() << " sensor returns");*/
    for (ISpatial::Result::iterator retrnIt = returnsToDraw.begin();
        retrnIt != returnsToDraw.end(); ++retrnIt) {
      Ref<UpdatedSensorReturns> retrn =
        retrnIt->dynamicCast<UpdatedSensorReturns>();
      if (retrn->getPerception() & perception_unit) {
        drawUnit(retrn->getUnit(), Colour::yellow);
      } else {
        Fatal("Not implemented");
      }
    }
    
    if (dragging) {
      /*Debug("Drawing the selection rectangle");*/
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
  /*Debug("posOfDisplayZero=" << posOfDisplayZero);*/
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

