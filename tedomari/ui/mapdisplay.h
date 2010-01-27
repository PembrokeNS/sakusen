#ifndef TEDOMARI__UI__MAPDISPLAY_H
#define TEDOMARI__UI__MAPDISPLAY_H

#include "point.h"
#include "game/game.h"
#include "ui/control.h"
#include "ui/surface.h"

namespace tedomari {
namespace ui {

class UI;

/** \brief Represents one view of a portion of the map on the UI.
 *
 * \todo Rethink this to take into account bizarre map topologies */
class MapDisplay : public Control {
  private:
    MapDisplay();
    MapDisplay(const MapDisplay&);
  public:
    MapDisplay(
        uint16 x,
        uint16 y,
        DockStyle ds,
        Region* r,
        tedomari::game::Game* g,
        UI* u,
        double initialScale
      ) :
      Control(x, y, ds, r), game(g), ui(u), posOfDisplayZero(),
      dexPerPixelX(initialScale), dexPerPixelY(initialScale), dragging(false)
    {}
  private:
    tedomari::game::Game* game; /* Not owned by this */
    UI* ui; /* Not owned by this */
    sakusen::Point<sint32> posOfDisplayZero;
    double dexPerPixelX;
    double dexPerPixelY;
    sakusen::Point<double> mousePos;
    bool dragging;
    sakusen::Point<sint32> dragStart;
    /** \brief A surface to be the ground beneath any units, cached because it
     * doesn't change much */
    Surface::Ptr groundImage;

    /* Methods to convert between coordinate systems */
    inline sakusen::Rectangle<double> dexToPixel(
        const sakusen::Rectangle<sint32>& r
      ) const;

    inline sakusen::Rectangle<sint32> pixelToDex(
        const sakusen::Rectangle<double>&
      ) const;
    
    inline sakusen::Point<double> dexToPixel(
        const sakusen::Point<sint32>&
      ) const;
    
    inline sakusen::Point<sint32> pixelToDex(
        const sakusen::Point<double>&
      ) const;

    void redrawGround();
    
    template<typename UnitRef>
    void drawUnit(const UnitRef& unit, const Colour&);
    void drawUnitOrders(
        const sakusen::Ref<const sakusen::client::UpdatedUnit>& unit,
        const Colour&
      );
    void drawRegion(
        const sakusen::Region<sint32>::ConstPtr& region,
        const Colour&
      );
    void drawBallistic(
        const sakusen::Ref<const sakusen::client::UpdatedBallistic>&,
        const Colour&
      );

  protected:
    void paint();
  public:
    inline double getDexWidth() const { return dexPerPixelX*getWidth(); }
    inline double getDexHeight() const { return dexPerPixelY*getHeight(); }
    inline sakusen::Position getMousePosAtGround() const {
      sakusen::Point<sint32> pos = pixelToDex(mousePos);
      const sakusen::Map* map = sakusen::world->getMap();
      map->resolvePosition(pos, pos);
      pos.z = map->getHeightfield().getHeightAt(pos);
      return pos;
    }
    inline sakusen::Position getMousePosAtZero() const {
      return pixelToDex(mousePos);
    }
    void update();
    void mouseMove(const sakusen::Point<double>&);
    void translate(const sakusen::Point<sint32>& d);
    void translate(sint32 dx, sint32 dy);
    void startDrag();
    sakusen::Rectangle<sint32> stopDrag();
};

inline sakusen::Rectangle<double> MapDisplay::dexToPixel(
    const sakusen::Rectangle<sint32>& r
  ) const {
  /* Note that the direction of increasing y is reversed during this
   * conversion */
  return sakusen::Rectangle<double>(
      (r.getMinX() - posOfDisplayZero.x)/dexPerPixelX,
      (posOfDisplayZero.y - r.getMaxY())/dexPerPixelY,
      (r.getMaxX() - posOfDisplayZero.x)/dexPerPixelX,
      (posOfDisplayZero.y - r.getMinY())/dexPerPixelY
    );
}

inline sakusen::Rectangle<sint32> MapDisplay::pixelToDex(
    const sakusen::Rectangle<double>& r
  ) const {
  /* The following is almost but not quite the smallest inter-aligned
   * rectangle bounding r in game unit space.  It might not be quite large
   * enough (by half a dex) if the lower bound is rounded down and the
   * height/width is not rounded up enough.  However, since a dex is probably
   * much smaller than a pixel, and doing it properly would require more
   * operations, I'm not worried about this small discrepancy */
  return sakusen::Rectangle<sint32>(
      sint32(floor(r.getMinX()*dexPerPixelX)) + posOfDisplayZero.x,
      posOfDisplayZero.y - sint32(ceil(r.getMaxY()*dexPerPixelY)),
      sint32(ceil(r.getMaxX()*dexPerPixelX)) + posOfDisplayZero.x,
      posOfDisplayZero.y - sint32(floor(r.getMinY()*dexPerPixelY))
    );
}

inline sakusen::Point<double> MapDisplay::dexToPixel(
    const sakusen::Point<sint32>& p
  ) const {
  return sakusen::Point<double>(
      (p.x-posOfDisplayZero.x)/dexPerPixelX,
      (posOfDisplayZero.y-p.y)/dexPerPixelY,
      0
    );
}

inline sakusen::Point<sint32> MapDisplay::pixelToDex(
    const sakusen::Point<double>& p
  ) const {
  return sakusen::Point<sint32>(
      sint32(round(p.x*dexPerPixelX))+posOfDisplayZero.x,
      posOfDisplayZero.y-sint32(round(p.y*dexPerPixelY)),
      0
    );
}

}}

#endif // TEDOMARI__UI__MAPDISPLAY_H

