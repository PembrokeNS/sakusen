#include "ui/sdl/sdlsurface.h"

#include "ui/sdl/masks.h"

namespace tedomari {
namespace ui {
namespace sdl {

SDLSurface::SDLSurface(uint16 width, uint16 height) :
  Surface(width, height)
{
  /* Create the SDL interface to that buffer */
  uint16 stride = getWidth() * TEDOMARI_UI_SDL_BYTES_PER_PIXEL;
  sdlSurface = SDL_CreateRGBSurfaceFrom(
      buffer, getWidth(), getHeight(), TEDOMARI_UI_SDL_BITS_PER_PIXEL, stride,
      TEDOMARI_UI_SDL_RMASK, TEDOMARI_UI_SDL_GMASK,
      TEDOMARI_UI_SDL_BMASK, TEDOMARI_UI_SDL_AMASK
    );
  if (sdlSurface == NULL) {
    SAKUSEN_FATAL(SDL_GetError());
  }
}

SDLSurface::~SDLSurface()
{
  SDL_FreeSurface(sdlSurface);
}

}}}

