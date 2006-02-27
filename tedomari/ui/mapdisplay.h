#ifndef UI__MAPDISPLAY_H
#define UI__MAPDISPLAY_H

#include "point.h"
#include "game/game.h"
#include "ui/control.h"

namespace tedomari {
namespace ui {

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
        tedomari::game::Game* g
      ) :
      Control(x, y, ds, r), game(g), posOfDisplayZero(),
      dexPerPixelX(1), dexPerPixelY(1)
    {}
  private:
    tedomari::game::Game* game; /* Not owned by this */
    sakusen::Point<sint32> posOfDisplayZero;
    double dexPerPixelX;
    double dexPerPixelY;

    /* Methods to convert between coordinate systems */
    inline Rectangle<double> dexToDisplay(const Rectangle<sint32>& r) {
      /* Note that the direction of increasing y is reversed during this
       * conversion */
      return Rectangle<double>(
          (r.x-posOfDisplayZero.x)/dexPerPixelX,
          (posOfDisplayZero.y-r.y-r.height)/dexPerPixelY,
          r.width/dexPerPixelX,
          r.height/dexPerPixelY
        );
    }
  protected:
    void paint();
  public:
    inline double getDexWidth() const { return dexPerPixelX*getWidth(); }
    inline double getDexHeight() const { return dexPerPixelY*getHeight(); }
    void translate(const sakusen::Point<sint32>& d);
    void translate(sint32 dx, sint32 dy);
};

}}

#endif // UI__MAPDISPLAY_H

