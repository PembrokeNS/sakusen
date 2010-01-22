#ifndef TEDOMARI__UI_SDL__SDLUI_H
#define TEDOMARI__UI_SDL__SDLUI_H

#include <optionsparser.h>

#include "ui/ui.h"
#include "ui/sdl/sdlsurface.h"

#ifdef NDEBUG
  #define TEDOMARI_UI_SDL_DEBUG(msg)
#else
  #define TEDOMARI_UI_SDL_DEBUG(msg) if (debug) { SAKUSEN_DEBUG(msg); }
#endif

struct SDL_Surface;

namespace tedomari {
namespace ui {
namespace sdl {

class SDLUI : public UI {
  public:
    struct Options {
      Options() :
        width(640), height(480), help(false), fullscreen(false),
        hwSurface(true), doubleBuffer(true), debug(false)
      {}
      int width;
      int height;
      bool help;
      bool fullscreen;
      bool hwSurface;
      bool doubleBuffer;
      bool debug;

      static void usage(std::ostream&);
    };
  private:
    SDLUI();
    SDLUI(const SDLUI&);
  public:
    SDLUI(const Options&, std::ifstream& uiConf, tedomari::game::Game*);
    ~SDLUI();
  protected:
#ifndef NDEBUG
    bool debug; /**< Should we print debugging info */
#endif
    bool doubleBuffer; /**< Are we using hardware double buffering */
    uint32 flags; /**< flags argument passed to SDL_SetVideoMode() */
    SDL_Surface* screen; /**< The SDL_Surface representing the output window */
    uint8* buffer; /**< The internal buffer to which we write, and copy to
                     screen.  Owned by this */
    size_t bufferLen;
    uint16 stride; /**< The offset between consecutive rows in buffer */
    SDL_Surface* sdlBuffer; /**< SDL's wrapper of buffer */

    virtual void resize(uint16 width, uint16 height);
  public:
    static optimal::OptionsParser getParser(Options* options);
    void setTitle(const String& title);
    void update();
    Surface::Ptr createSurface(uint16 width, uint16 height);
    void blit(uint16 x, uint16 y, const SDLSurface::ConstPtr&);

    uint32 getSDLColour(const Colour&) const;
};

}}}

#endif // TEDOMARI__UI_SDL__SDLUI_H

