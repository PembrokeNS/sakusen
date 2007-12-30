#ifndef UI__DUMMYREGION_H
#define UI__DUMMYREGION_H

namespace tedomari {
namespace ui {

/** \brief This method implements most of the methods of Region with stubs
 * which do nothing. */
class DummyRegion : public Region {
  private:
    DummyRegion();
    DummyRegion(const DummyRegion&);
  public:
    DummyRegion(uint16 width, uint16 height) : Region(width, height) {}
    inline void paint() const {}
    inline Region* newSubRegion(uint16, uint16, uint16 width, uint16 height)
      const {
      return new DummyRegion(width, height);
    }
    sakusen::Point<double> globalToLocal(const sakusen::Point<double>& global) {
      Fatal("not implemented");
      return global; //Make VC happy
    }
    sakusen::Point<double> localToGlobal(const sakusen::Point<double>& local) {
      Fatal("not implemented");
      return local; //Make VC happy
    }
    Layout* newLayout() const { 
      Fatal("not implemented"); 
      return 0; //Make VC happy
    }
    void blit(const Surface::ConstPtr&) {}
    void setClip() {}
    void unsetClip() {}
    void fill(const Colour&) {}
    void fillRect(double, double, double, double, const Colour&) {}
    void fillCircle(double, double, double, const Colour&) {}
    void fillPolygon(
        const std::list< sakusen::Point<double> >&,
        const Colour&
      ) {}
    void stroke(double, double, double, double, const Colour&) {}
    void drawRect(double, double, double, double, const Colour&) {}
    void drawText(double, double, const String&, const Colour&) {}
    void drawText(double, double, const Layout*) {}
};

}}

#endif // UI__DUMMYREGION_H

