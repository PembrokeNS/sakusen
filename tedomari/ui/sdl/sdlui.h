#ifndef TEDOMARI__UI_SDL__SDLUI_H
#define TEDOMARI__UI_SDL__SDLUI_H

#include <optionsparser.h>

#include "ui/ui.h"

struct SDL_Surface;
typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;
typedef struct _PangoLayout PangoLayout;
typedef struct _PangoContext PangoContext;

namespace tedomari {
namespace ui {
namespace sdl {

class SDLLayout;

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
    SDLUI(const Options&, std::ifstream& uiConf, tedomari::game::Game*);
    ~SDLUI();
  private:
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
    cairo_surface_t* cairoBuffer; /**< cairo's wrapper of buffer */
    cairo_t* cairoContext; /**< Context for cairoBuffer */
    PangoContext* pangoContext;
  protected:
    void resize(uint16 width, uint16 height);
    Region* newRegion(uint16 x, uint16 y, uint16 width, uint16 height);
  public:
    static optimal::OptionsParser getParser(Options* options);
    void setTitle(const String& title);
    void update();

    PangoLayout* newPangoLayout();

    void cairoSetSource(const Colour& c);
    uint32 getSDLColour(const Colour&);
    void setClipRect(double x, double y, double w, double h);
    void resetClip();
    void fillRect(double x, double y, double w, double h, const Colour&);
    void stroke(double x1, double y1, double x2, double y2, const Colour&);
    void drawText(double x, double y, const String& text, const Colour&);
    void drawText(double x, double y, const SDLLayout* layout);
};

}}}

#endif // TEDOMARI__UI_SDL__SDLUI_H

