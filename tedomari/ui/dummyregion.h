#ifndef UI__DUMMYREGION_H
#define UI__DUMMYREGION_H

namespace tedomari {
namespace ui {

class DummyRegion : public Region {
  private:
    DummyRegion();
    DummyRegion(const DummyRegion&);
  public:
    DummyRegion(uint16 width, uint16 height) : Region(width, height) {}
    inline Region* newSubRegion(uint16, uint16, uint16 width, uint16 height) {
      return new DummyRegion(width, height);
    }
    Layout* newLayout() { Fatal("not implemented"); }
    void flood(Colour) {}
    void stroke(double, double, double, double, const Colour&) {}
    void drawText(double, double, const String&, const Colour&) {}
    void drawText(double, double, const Layout*) {}
};

}}

#endif // UI__DUMMYREGION_H

