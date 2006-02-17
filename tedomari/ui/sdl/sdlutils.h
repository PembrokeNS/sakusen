#ifndef UI_SDL__SDLUTILS_H
#define UI_SDL__SDLUTILS_H

#include "libsakusen-global.h"

#include <SDL/SDL.h>

#include "ui/button.h"
#include "ui/key.h"

namespace tedomari {
namespace ui {
namespace sdl {

Button sdlUtils_getButton(uint8 b);

Key sdlUtils_getKey(SDLKey);

}}}

#endif // UI_SDL__SDLUTILS_H

