#ifndef UI__MAPDISPLAY_H
#define UI__MAPDISPLAY_H

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
      Control(x, y, ds, r), game(g) {}
  private:
    tedomari::game::Game* game; /* Not owned by this */
  protected:
    void paint();
};

}}

#endif // UI__MAPDISPLAY_H

