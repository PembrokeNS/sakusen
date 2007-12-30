#ifndef TEDOMARI__UI_SDL__SDLSURFACE_H
#define TEDOMARI__UI_SDL__SDLSURFACE_H

#include <SDL/SDL.h>

#include "ui/surface.h"

namespace tedomari {
namespace ui {
namespace sdl {

class SDLUI;

class SDLSurface : public Surface {
  friend class SDLUI;
  public:
    typedef boost::shared_ptr<SDLSurface> Ptr;
    typedef boost::shared_ptr<const SDLSurface> ConstPtr;
    
    SDLSurface(uint16 width, uint16 height);
    ~SDLSurface();
  private:
    SDL_Surface* sdlSurface;
};

}}}

#endif // TEDOMARI__UI_SDL_CAIRO__CAIROSURFACE_H

