#include "ui/sdl/sdlsurface.h"

#include "ui/sdl/masks.h"

namespace tedomari {
namespace ui {
namespace sdl {

SDLSurface::SDLSurface(uint16 width, uint16 height) :
  Surface(width, height)
{
  /* Create the SDL interface to that buffer */
  uint16 stride = getWidth() * BYTES_PER_PIXEL;
  sdlSurface = SDL_CreateRGBSurfaceFrom(
      buffer, getWidth(), getHeight(), BITS_PER_PIXEL, stride, RMASK,
      GMASK, BMASK, AMASK
    );
  if (sdlSurface == NULL) {
    Fatal(SDL_GetError());
  }
}

SDLSurface::~SDLSurface()
{
  SDL_FreeSurface(sdlSurface);
}

}}}

