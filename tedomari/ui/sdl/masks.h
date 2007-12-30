#ifndef TEDOMARI__UI_SDL__MASKS_H
#define TEDOMARI__UI_SDL__MASKS_H

#include <SDL/SDL.h>

#define BYTES_PER_PIXEL (4)
#define BITS_PER_PIXEL (8*BYTES_PER_PIXEL)

/* These mask choices are needed to share the buffer between SDL and cairo */
#define AMASK 0xff000000
#define RMASK 0x00ff0000
#define GMASK 0x0000ff00
#define BMASK 0x000000ff
/*
 * The following mask choices would supposedly be required to share the buffer
 * between SDL and OpenGL.  Given that the first one goes R,B,G, I find this
 * most implausible
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  #define RMASK 0xff000000
  #define BMASK 0x00ff0000
  #define GMASK 0x0000ff00
  #define AMASK 0x000000ff
#else
  #define RMASK 0x000000ff
  #define GMASK 0x0000ff00
  #define BMASK 0x00ff0000
  #define AMASK 0xff000000
#endif
*/

#endif // TEDOMARI__UI_SDL__MASKS_H

