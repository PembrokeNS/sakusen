#include "sdlui.h"

#include "libsakusen-global.h"
#include "ui/dummyregion.h"
#include "ui/sdl/sdlutils.h"

#include <SDL/SDL.h>

#include <iomanip>

using namespace std;
using namespace optimal;

using namespace tedomari::game;
using namespace tedomari::ui;
using namespace tedomari::ui::sdl;

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

SDLUI::SDLUI(const Options& options, ifstream& uiConf, Game* game) :
  UI(new DummyRegion(options.width, options.height), uiConf, game),
  doubleBuffer(false)
{
#ifdef NDEBUG
  if (options.debug) {
    Debug("SDL debugging output not supported on release build");
  }
  debug = false;
#else
  debug = options.debug;
#endif
  SDLDebug("Enabled SDL debugging output");
  
  /* Initialize SDL */
  if (-1 == SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    Fatal(SDL_GetError());
  }

  /* Turn on returning unicode values with keydown events */
  SDL_EnableUNICODE(true);

  /* Set the video mode */
  flags = SDL_RESIZABLE|SDL_ANYFORMAT;
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
  
  screen = SDL_SetVideoMode(getWidth(), getHeight(), BITS_PER_PIXEL, flags);
  if (screen == NULL) {
    Fatal(SDL_GetError());
  }

  /* Create our internal buffer */
  stride = getWidth() * BYTES_PER_PIXEL;
  bufferLen = stride * getHeight();
  buffer = new uint8[bufferLen];

  /* Create the SDL interface to that buffer */
  sdlBuffer = SDL_CreateRGBSurfaceFrom(
      buffer, getWidth(), getHeight(), BITS_PER_PIXEL, stride, RMASK,
      GMASK, BMASK, AMASK
    );
  if (sdlBuffer == NULL) {
    Fatal(SDL_GetError());
  }
}

SDLUI::~SDLUI()
{
  SDL_FreeSurface(sdlBuffer);
  SDL_Quit();
  delete[] buffer;
}

void SDLUI::resize(uint16 width, uint16 height)
{
  /* Get the new screen surface */
  screen = SDL_SetVideoMode(width, height, BITS_PER_PIXEL, flags);
  /* Create new internal buffer */
  stride = width * BYTES_PER_PIXEL;
  delete[] buffer;
  bufferLen = stride * height;
  buffer = new uint8[bufferLen];
  /* Create new SDL interface to buffer */
  SDL_FreeSurface(sdlBuffer);
  sdlBuffer = SDL_CreateRGBSurfaceFrom(
      buffer, width, height, BITS_PER_PIXEL, stride, RMASK, GMASK, BMASK, AMASK
    );
  if (sdlBuffer == NULL) {
    Fatal(SDL_GetError());
  }
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
  /* Poll for events */
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_ACTIVEEVENT:
        SDLDebug("SDL_ACTIVEEVENT");
        /** \todo Something appropriate.  When minimizing, we should lower CPU
         * consumption appropriately.  Also we need to consider the status of
         * modifier keys (e.g. when ctrl is released when the UI does not have
         * focus) */
        break;
      case SDL_VIDEOEXPOSE:
        SDLDebug("SDL_VIDEOEXPOSE");
        /** \todo Redraw the screen */
        break;
      case SDL_VIDEORESIZE:
        SDLDebug("SDL_VIDEORESIZE");
        resize(event.resize.w, event.resize.h);
        break;
      case SDL_KEYDOWN:
        SDLDebug(
            "SDL_KEYDOWN: " << SDL_GetKeyName(event.key.keysym.sym) <<
            ", char: " << std::hex << std::setw(4) <<
            event.key.keysym.unicode << std::dec
          );
        keyDown(
            sdlUtils_getKey(event.key.keysym.sym), event.key.keysym.unicode
          );
        break;
      case SDL_KEYUP:
        SDLDebug("SDL_KEYUP: " << SDL_GetKeyName(event.key.keysym.sym));
        keyUp(sdlUtils_getKey(event.key.keysym.sym));
        break;
      case SDL_MOUSEBUTTONDOWN:
        {
          SDL_MouseButtonEvent mouse = event.button;
          SDLDebug("SDL_MOUSEDOWN: button=" << sint32(mouse.button));
          keyDown(sdlUtils_getButton(mouse.button), 0);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        {
          SDL_MouseButtonEvent mouse = event.button;
          SDLDebug("SDL_MOUSEUP: button=" << sint32(mouse.button));
          keyUp(sdlUtils_getButton(mouse.button));
        }
        break;
      case SDL_QUIT:
        SDLDebug("SDL_QUIT");
        quitRequest();
        break;
      case SDL_MOUSEMOTION:
        {
          SDL_MouseMotionEvent mouse = event.motion;
          mouseMove(sakusen::Point<double>(mouse.x, mouse.y, 0));
        }
        break;
      default:
        Debug("Unexpected SDL_Event type: " << sint32(event.type));
        break;
    }
  }

  /* Call superclass update method */
  UI::update();
  
  /* Update the display */
  /** \todo Update only that portion which has been altered */
  if (0 != SDL_BlitSurface(sdlBuffer, NULL, screen, NULL)) {
    Debug("SDL_BlitSurface failed: " << SDL_GetError());
  }
  if (SDL_Flip(screen)) {
    Fatal(SDL_GetError());
  }
}

uint32 SDLUI::getSDLColour(const Colour& c)
{
  return SDL_MapRGBA(sdlBuffer->format, c.ir(), c.ig(), c.ib(), c.ia());
}

