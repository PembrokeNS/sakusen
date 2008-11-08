#include "ui/mapdisplay.h"

#include "heightfield-methods.h"
#include "unitcorneriterator.h"
#include "ui/ui.h"

using namespace std;

using namespace sakusen;
using namespace sakusen::client;

using namespace tedomari::ui;

/** \brief Refresh the cached image of the ground. */
void MapDisplay::redrawGround()
{
  Surface& g = *groundImage;
  const Map* m = client::world->getMap();
  const sint32 left = m->left();
  const sint32 right = m->right();
  const sint32 bottom = m->bottom();
  const sint32 top = m->top();
  const Heightfield& heightfield =
    dynamic_cast<const Heightfield&>(m->getHeightfield());
  const uint16 width = getWidth();
  const uint16 height = getHeight();
  assert(width == g.getWidth());
  assert(height == g.getHeight());
  const boost::tuple<uint8, uint8, uint8> red(255, 0, 0);

  Point<uint16> pixel(0,0,0);
  Point<double> ddex;

  /** \todo The following loop is a serious CPU bottleneck, so it's been
   * somewhat hand-optimized.  In the long run we want to do something
   * sensible like actually use the capabilities of the graphics card. */
  for (pixel.x=0; pixel.x<width; ++pixel.x) {
    pixel.y=0;
    ddex = pixelToDex(pixel);
    if (ddex.x < left || ddex.x >= right) {
      for (; pixel.y<height; ++pixel.y) {
        g(pixel.x,pixel.y) = red;
      }
      continue;
    }
    for (; pixel.y<height; ++pixel.y, ddex.y-=dexPerPixelY) {
      if (ddex.y < bottom || ddex.y >= top) {
        g(pixel.x,pixel.y) = red;
      } else {
        sint32 z = heightfield.getApproxHeightAt(Point<sint32>(ddex));
        //QDebug("height("<<dex.x<<", "<<dex.y<<")="<<height);
        /** \bug A host of arbitrary constants. */
        /* crop */
        if (z >= (1<<16)) {
          z = (1<<16)-1;
        }
        if (z < -(1<<16)) {
          z = -(1<<16);
        }
        /* convert this height into the range [0,256) */
        z += (1<<16);
        z /= (1<<9);
        g(pixel.x,pixel.y) =
          boost::make_tuple(uint8(0), uint8(255-z), uint8(z));
      }
    }
  }
}

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

  for (UnitCornerIterator corner(*unit, true), end; corner != end; ++corner) {
    corners.push_back(dexToPixel(*corner));
  }
  assert(unit);
  getRegion()->fillPolygon(corners, colour);
}

/** \brief Draw a graphical representation of a unit's orders.
 * \param[in] unit The unit to draw. Must not be NULL.
 * \param[in] colour The colour of the player who owns this unit.
 */
void MapDisplay::drawUnitOrders(
    const Ref<const sakusen::client::UpdatedUnit>& unit,
    const Colour& colour
  )
{
  Point<double> targetPosition;
  Point<double> currentPosition;
  UnitOrders uo = unit->getUnitOrders();

  if (uo.getLinearTarget() == linearTargetType_position) {
    targetPosition = dexToPixel(uo.getTargetPosition());
    currentPosition = dexToPixel(unit->getIStatus().getPosition());
    getRegion()->stroke(currentPosition.x, currentPosition.y,
        targetPosition.x, targetPosition.y, colour);
  }
}

void MapDisplay::drawBallistic(
    const Ref<const sakusen::client::UpdatedBallistic>& ballistic,
    const Colour& colour
  )
{
  Position dexPos = ballistic->getPath().evaluate(sakusen::world->getTimeNow());
  Point<double> pixelPos = dexToPixel(dexPos);
  getRegion()->fillCircle(
      pixelPos,
      3 /** \bug Arbitrary number for ballistic radius */,
      colour
    );
}

void MapDisplay::paint()
{
  Region* r = getRegion();
  if (!game->isStarted()) {
    /* When game not started, we flood with green and print a diagonal red
     * stripe, as a proof of concept. */
    r->fill(Colour::green);
    r->stroke(0, 0, r->getWidth(), r->getHeight(), Colour::red);
  } else {
    /** \todo obviously we need a bit more functionality */
    r->setClip();
    /* Red background */
    r->fill(Colour::red);
    /*Map* m = sakusen::world->getMap();*/
    /* Update the ground image if we've been resized */
    if (!groundImage ||
        groundImage->getWidth() != getWidth() ||
        groundImage->getHeight() != getHeight()) {
      groundImage = ui->createSurface(getWidth(), getHeight());
      redrawGround();
    }
    r->blit(groundImage);
    /*
    r->fillRect(dexToPixel(
          Rectangle<sint32>(m->left(), m->bottom(), m->right(), m->top())
        ), Colour::green);
        */
    Rectangle<sint32> displayRect = pixelToDex(
        Rectangle<double>(0, 0, getWidth(), getHeight())
      );
    
    /* Draw units */
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
    
    /* Draw sensor returns */
    ISpatial::Result returnsToDraw =
      sakusen::client::world->getSpatialIndex()->findIntersecting(
          displayRect, gameObject_sensorReturns
        );
    /*QDebug("drawing " << returnsToDraw.size() << " sensor returns");*/
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

    /* Draw ballistics */
    ISpatial::Result ballisticsToDraw =
      sakusen::client::world->getSpatialIndex()->findIntersecting(
          displayRect, gameObject_ballistic
        );
    /*QDebug("drawing " << ballisticsToDraw.size() << " ballistics");*/
    for (ISpatial::Result::iterator ballisticIt = ballisticsToDraw.begin();
        ballisticIt != ballisticsToDraw.end(); ++ballisticIt) {
      Ref<UpdatedBallistic> ballistic =
        ballisticIt->dynamicCast<UpdatedBallistic>();
      drawBallistic(ballistic, Colour::yellow*0.8);
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
  getRegion()->paint();
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
  redrawGround();
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

