#ifndef TEDOMARI__UI_SDL__SDLUI_H
#define TEDOMARI__UI_SDL__SDLUI_H

#include "ui/ui.h"

struct SDL_Surface;

namespace tedomari {
namespace ui {
namespace sdl {

class SDLUI : public UI {
  public:
    SDLUI();
    ~SDLUI();
  private:
    SDL_Surface* screen;
};

}}}

#endif // TEDOMARI__UI_SDL__SDLUI_H

