#ifndef UI_SDL__SDLLAYOUT_H
#define UI_SDL__SDLLAYOUT_H

#include "ui/layout.h"
#include "ui/sdl/sdlui.h"

typedef struct _PangoLayout PangoLayout;

namespace tedomari {
namespace ui {
namespace sdl {

class SDLLayout : public Layout {
  private:
    SDLLayout();
    SDLLayout(const SDLLayout&);
  public:
    SDLLayout(SDLUI*);
    ~SDLLayout();
  private:
    SDLUI* ui;
    PangoLayout* layout;
  public:
    void setText(const String& t);
    uint16 getHeight() const;
    inline PangoLayout* getLayout() const { return layout; }

    void contextChanged();
};

}}}

#endif // UI_SDL__SDLLAYOUT_H

