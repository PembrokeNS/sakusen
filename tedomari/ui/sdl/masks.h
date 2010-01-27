#ifndef TEDOMARI__UI_SDL__MASKS_H
#define TEDOMARI__UI_SDL__MASKS_H

#include <SDL/SDL.h>

#define TEDOMARI_UI_SDL_BYTES_PER_PIXEL (4)
#define TEDOMARI_UI_SDL_BITS_PER_PIXEL (8*TEDOMARI_UI_SDL_BYTES_PER_PIXEL)

/* These mask choices are needed to share the buffer between SDL and cairo */
#define TEDOMARI_UI_SDL_AMASK 0xff000000
#define TEDOMARI_UI_SDL_RMASK 0x00ff0000
#define TEDOMARI_UI_SDL_GMASK 0x0000ff00
#define TEDOMARI_UI_SDL_BMASK 0x000000ff
/*
 * The following mask choices would supposedly be required to share the buffer
 * between SDL and OpenGL.  Given that the first one goes R,B,G, I find this
 * most implausible
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  #define TEDOMARI_UI_SDL_RMASK 0xff000000
  #define TEDOMARI_UI_SDL_BMASK 0x00ff0000
  #define TEDOMARI_UI_SDL_GMASK 0x0000ff00
  #define TEDOMARI_UI_SDL_AMASK 0x000000ff
#else
  #define TEDOMARI_UI_SDL_RMASK 0x000000ff
  #define TEDOMARI_UI_SDL_GMASK 0x0000ff00
  #define TEDOMARI_UI_SDL_BMASK 0x00ff0000
  #define TEDOMARI_UI_SDL_AMASK 0xff000000
#endif
*/

#endif // TEDOMARI__UI_SDL__MASKS_H

