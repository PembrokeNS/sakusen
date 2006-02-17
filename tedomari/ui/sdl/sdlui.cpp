#include "sdlui.h"

#include "libsakusen-global.h"
#include "sdlutils.h"

#include <SDL/SDL.h>

#include <iomanip>

using namespace optimal;

using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

#ifdef NDEBUG
  #define SDLDebug(msg)
#else
  #define SDLDebug(msg) if (debug) { Debug(msg); }
#endif

SDLUI::SDLUI(const Options& options) :
  doubleBuffer(false)
{
  if (-1 == SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    Fatal(SDL_GetError());
  }

  uint32 flags = SDL_ANYFORMAT|SDL_RESIZABLE;
  if (options.fullscreen) {
    flags |= SDL_FULLSCREEN;
  }
  if (options.hwSurface) {
    flags |= SDL_HWSURFACE;
    if (options.doubleBuffer) {
      flags |= SDL_DOUBLEBUF;
      doubleBuffer = true;
    }
  } else {
    flags |= SDL_SWSURFACE;
  }
  
  screen = SDL_SetVideoMode(options.width, options.height, 24, flags);
  if (screen == NULL) {
    Fatal(SDL_GetError());
  }

  SDL_EnableUNICODE(true);

#ifdef NDEBUG
  if (options.debug) {
    Debug("SDL debugging output not supported on release build");
  }
  debug = false;
#else
  debug = options.debug;
  SDLDebug("Enabled SDL debugging output");
#endif
}

SDLUI::~SDLUI()
{
  SDL_Quit();
}

OptionsParser SDLUI::getParser(Options* options)
{
  OptionsParser parser(',');
  
  parser.addOption("w",             '\0', &options->width);
  parser.addOption("h",             '\0', &options->height);
  parser.addOption("fs",            '\0', &options->fullscreen);
  parser.addOption("hw-surface",    '\0', &options->hwSurface);
  parser.addOption("double-buffer", '\0', &options->doubleBuffer);
  parser.addOption("debug",         '\0', &options->debug);

  return parser;
}

void SDLUI::setTitle(const String& title)
{
  SDL_WM_SetCaption(title.c_str(), NULL);
}

void SDLUI::update()
{
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_ACTIVEEVENT:
        SDLDebug("SDL_ACTIVEEVENT");
        /* TODO: Something appropriate.  When minimizing, we should lower CPU
         * consumption appropriately */
        break;
      case SDL_VIDEOEXPOSE:
        SDLDebug("SDL_VIDEOEXPOSE");
        /* TODO: redraw the screen */
        break;
      case SDL_VIDEORESIZE:
        SDLDebug("SDL_VIDEORESIZE");
        resize(event.resize.w, event.resize.h);
        break;
      case SDL_KEYDOWN:
        SDLDebug(
            "SDL_KEYDOWN: " << SDL_GetKeyName(event.key.keysym.sym) <<
            ", char: " << std::hex << std::setw(4) << event.key.keysym.unicode
          );
        if (isExpectingChars() && 0 != event.key.keysym.unicode) {
          charInput(event.key.keysym.unicode);
        } else {
          keyDown(sdlUtils_getKey(event.key.keysym.sym));
        }
        break;
      case SDL_KEYUP:
        SDLDebug("SDL_KEYUP: " << SDL_GetKeyName(event.key.keysym.sym));
        if (!isExpectingChars()) {
          keyUp(sdlUtils_getKey(event.key.keysym.sym));
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        {
          SDL_MouseButtonEvent mouse = event.button;
          SDLDebug("SDL_MOUSEDOWN: button=" << sint32(mouse.button));
          mouseDown(sdlUtils_getButton(mouse.button), mouse.x, mouse.y);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        {
          SDL_MouseButtonEvent mouse = event.button;
          SDLDebug("SDL_MOUSEUP: button=" << sint32(mouse.button));
          mouseUp(sdlUtils_getButton(mouse.button), mouse.x, mouse.y);
        }
        break;
      case SDL_QUIT:
        SDLDebug("SDL_QUIT");
        quitRequest();
        break;
      case SDL_MOUSEMOTION:
        break;
      default:
        Debug("Unexpected SDL_Event type: " << sint32(event.type));
        break;
    }
  }
}

