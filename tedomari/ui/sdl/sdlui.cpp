#include "sdlui.h"

#include "libsakusen-global.h"

#include "SDL/SDL.h"

using namespace tedomari::ui::sdl;

SDLUI::SDLUI()
{
  if (-1 == SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    Fatal(SDL_GetError());
  }
  
  screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE|SDL_ANYFORMAT);
  if (screen == NULL) {
    Fatal(SDL_GetError());
  }
}

SDLUI::~SDLUI()
{
  SDL_Quit();
}
