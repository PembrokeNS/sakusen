#ifndef TEDOMARI__UI_SDL__SDLUTILS_H
#define TEDOMARI__UI_SDL__SDLUTILS_H

#include "libsakusen-global.h"

#include <SDL/SDL.h>

#include "ui/key.h"

namespace tedomari {
namespace ui {
namespace sdl {

Key sdlUtils_getButton(uint8 b);

Key sdlUtils_getKey(SDLKey);

}}}

#endif // TEDOMARI__UI_SDL__SDLUTILS_H

