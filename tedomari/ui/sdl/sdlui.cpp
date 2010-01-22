#include "sdlui.h"

#include "libsakusen-global.h"
#include "ui/dummyregion.h"
#include "ui/sdl/sdlutils.h"
#include "ui/sdl/masks.h"

#include <SDL/SDL.h>

#include <iomanip>

using namespace std;
using namespace optimal;

using namespace tedomari::game;

namespace tedomari {
namespace ui {
namespace sdl {

void SDLUI::Options::usage(std::ostream& out)
{
  out <<
"SDL UI options (comma separated):\n"
" help              Display this message.\n"
" w=WIDTH           Set display width.\n"
" h=HEIGHT          Set display height.\n"
" fs                Use fullscreen mode.\n"
" hw-surface=no     Don't try to use a hardware surface for rendering.\n"
" double-buffer=no  Don't try to use a double buffer for rendering.\n"
" debug             Output extra debugging info.\n"
<< endl;
}

SDLUI::SDLUI(const Options& options, ifstream& uiConf, Game* game) :
  UI(new DummyRegion(options.width, options.height), uiConf, game),
  doubleBuffer(false)
{
#ifdef NDEBUG
  if (options.debug) {
    SAKUSEN_DEBUG("SDL debugging output not supported on release build");
  }
#else
  debug = options.debug;
#endif
  TEDOMARI_UI_SDL_DEBUG("Enabled SDL debugging output");
  
  /* Initialize SDL */
  if (-1 == SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SAKUSEN_FATAL(SDL_GetError());
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
  
  screen = SDL_SetVideoMode(getWidth(), getHeight(), TEDOMARI_UI_SDL_BITS_PER_PIXEL, flags);
  if (screen == NULL) {
    SAKUSEN_FATAL(SDL_GetError());
  }

  /* Create our internal buffer */
  stride = getWidth() * TEDOMARI_UI_SDL_BYTES_PER_PIXEL;
  bufferLen = stride * getHeight();
  buffer = new uint8[bufferLen];

  /* Create the SDL interface to that buffer */
  sdlBuffer = SDL_CreateRGBSurfaceFrom(
      buffer, getWidth(), getHeight(), TEDOMARI_UI_SDL_BITS_PER_PIXEL, stride,
      TEDOMARI_UI_SDL_RMASK, TEDOMARI_UI_SDL_GMASK,
      TEDOMARI_UI_SDL_BMASK, TEDOMARI_UI_SDL_AMASK
    );
  if (sdlBuffer == NULL) {
    SAKUSEN_FATAL(SDL_GetError());
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
  screen =
    SDL_SetVideoMode(width, height, TEDOMARI_UI_SDL_BITS_PER_PIXEL, flags);
  /* Create new internal buffer */
  stride = width * TEDOMARI_UI_SDL_BYTES_PER_PIXEL;
  delete[] buffer;
  bufferLen = stride * height;
  buffer = new uint8[bufferLen];
  /* Create new SDL interface to buffer */
  SDL_FreeSurface(sdlBuffer);
  sdlBuffer = SDL_CreateRGBSurfaceFrom(
      buffer, width, height, TEDOMARI_UI_SDL_BITS_PER_PIXEL, stride,
      TEDOMARI_UI_SDL_RMASK, TEDOMARI_UI_SDL_GMASK,
      TEDOMARI_UI_SDL_BMASK, TEDOMARI_UI_SDL_AMASK
    );
  if (sdlBuffer == NULL) {
    SAKUSEN_FATAL(SDL_GetError());
  }
}

OptionsParser SDLUI::getParser(Options* options)
{
  OptionsParser parser(',');
  
  parser.addOption("w",             '\0', &options->width);
  parser.addOption("h",             '\0', &options->height);
  parser.addOption("help",          '\0', &options->help);
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
        TEDOMARI_UI_SDL_DEBUG("SDL_ACTIVEEVENT");
        /** \todo Something appropriate.  When minimizing, we should lower CPU
         * consumption appropriately.  Also we need to consider the status of
         * modifier keys (e.g. when ctrl is released when the UI does not have
         * focus) */
        break;
      case SDL_VIDEOEXPOSE:
        TEDOMARI_UI_SDL_DEBUG("SDL_VIDEOEXPOSE");
        /** \todo Redraw the screen */
        break;
      case SDL_VIDEORESIZE:
        TEDOMARI_UI_SDL_DEBUG("SDL_VIDEORESIZE");
        resize(event.resize.w, event.resize.h);
        break;
      case SDL_KEYDOWN:
        TEDOMARI_UI_SDL_DEBUG(
            "SDL_KEYDOWN: " << SDL_GetKeyName(event.key.keysym.sym) <<
            ", char: " << std::hex << std::setw(4) <<
            event.key.keysym.unicode << std::dec
          );
        keyDown(
            sdlUtils_getKey(event.key.keysym.sym), event.key.keysym.unicode
          );
        break;
      case SDL_KEYUP:
        TEDOMARI_UI_SDL_DEBUG(
            "SDL_KEYUP: " << SDL_GetKeyName(event.key.keysym.sym)
          );
        keyUp(sdlUtils_getKey(event.key.keysym.sym));
        break;
      case SDL_MOUSEBUTTONDOWN:
        {
          SDL_MouseButtonEvent mouse = event.button;
          TEDOMARI_UI_SDL_DEBUG(
              "SDL_MOUSEDOWN: button=" << sint32(mouse.button)
            );
          keyDown(sdlUtils_getButton(mouse.button), 0);
        }
        break;
      case SDL_MOUSEBUTTONUP:
        {
          SDL_MouseButtonEvent mouse = event.button;
          TEDOMARI_UI_SDL_DEBUG(
              "SDL_MOUSEUP: button=" << sint32(mouse.button)
            );
          keyUp(sdlUtils_getButton(mouse.button));
        }
        break;
      case SDL_QUIT:
        TEDOMARI_UI_SDL_DEBUG("SDL_QUIT");
        quitRequest();
        break;
      case SDL_MOUSEMOTION:
        {
          SDL_MouseMotionEvent mouse = event.motion;
          mouseMove(sakusen::Point<double>(mouse.x, mouse.y, 0));
        }
        break;
      default:
        SAKUSEN_DEBUG("Unexpected SDL_Event type: " << sint32(event.type));
        break;
    }
  }

  /* Call superclass update method */
  UI::update();
  
  /* Update the display */
  /** \todo Update only that portion which has been altered */
  if (0 != SDL_BlitSurface(sdlBuffer, NULL, screen, NULL)) {
    SAKUSEN_DEBUG("SDL_BlitSurface failed: " << SDL_GetError());
  }
  if (SDL_Flip(screen)) {
    SAKUSEN_FATAL(SDL_GetError());
  }
}

Surface::Ptr SDLUI::createSurface(uint16 width, uint16 height)
{
  return Surface::Ptr(new SDLSurface(width, height));
}

void SDLUI::blit(uint16 x, uint16 y, const SDLSurface::ConstPtr& surface)
{
  SDL_Surface* src = surface->sdlSurface;
  /* The destination rectangle width and height do not matter */
  SDL_Rect destRect = { /*.x=*/x, /*.y=*/y, /*.w=*/0, /*.h=*/0 };
  if (SDL_BlitSurface(src, NULL, sdlBuffer, &destRect)) {
    SAKUSEN_FATAL(SDL_GetError());
  }
}

uint32 SDLUI::getSDLColour(const Colour& c) const
{
  return SDL_MapRGBA(sdlBuffer->format, c.ir(), c.ig(), c.ib(), c.ia());
}

}}}

