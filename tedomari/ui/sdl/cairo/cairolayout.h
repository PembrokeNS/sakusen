#ifndef UI_SDL_CAIRO__CAIROLAYOUT_H
#define UI_SDL_CAIRO__CAIROLAYOUT_H

#include "ui/layout.h"
#include "ui/sdl/cairo/cairoui.h"

typedef struct _PangoLayout PangoLayout;

namespace tedomari {
namespace ui {
namespace sdl {
namespace cairo {

class CairoLayout : public Layout {
  private:
    CairoLayout();
    CairoLayout(const CairoLayout&);
  public:
    CairoLayout(CairoUI*);
    ~CairoLayout();
  private:
    CairoUI* ui;
    PangoLayout* layout;
  public:
    void setText(const String& t);
    uint16 getHeight() const;
    inline PangoLayout* getLayout() const { return layout; }

    void contextChanged();
};

}}}}

#endif // UI_SDL_CAIRO__CAIROLAYOUT_H

