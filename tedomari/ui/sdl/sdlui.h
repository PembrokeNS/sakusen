#ifndef TEDOMARI__UI_SDL__SDLUI_H
#define TEDOMARI__UI_SDL__SDLUI_H

#include "optionsparser.h"

#include "ui/ui.h"

struct SDL_Surface;

namespace tedomari {
namespace ui {
namespace sdl {

class SDLUI : public UI {
  public:
    struct Options {
      Options() :
        width(640), height(480), fullscreen(false), hwSurface(true),
        doubleBuffer(true), debug(false) {}
      int width;
      int height;
      bool fullscreen;
      bool hwSurface;
      bool doubleBuffer;
      bool debug;
    };
  private:
    SDLUI();
    SDLUI(const SDLUI&);
  public:
    SDLUI(const Options&);
    ~SDLUI();
  private:
#ifndef NDEBUG
    bool debug; /**< Should we print debugging info */
#endif
    bool doubleBuffer; /**< Are we using hardware double buffering */
    SDL_Surface* screen;
  public:
    static optimal::OptionsParser getParser(Options* options);
    void setTitle(const String& title);
    void update();
};

}}}

#endif // TEDOMARI__UI_SDL__SDLUI_H

