#ifndef TEDOMARI__UI__SURFACE_H
#define TEDOMARI__UI__SURFACE_H

#include "tedomari-global.h"

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#include "ui/colour.h"

namespace tedomari {
namespace ui {

/* \brief A graphics surface stored in memory */
class Surface : private boost::noncopyable {
  public:
    typedef boost::shared_ptr<Surface> Ptr;
    typedef boost::shared_ptr<const Surface> ConstPtr;

    class SurfacePixel {
      friend class Surface;
      private:
        SurfacePixel(uint8* p) : pos(p) {}
        uint8* pos;
      public:
        void operator=(const Colour& c) {
          /** \bug Hard-coded colour masking; the correct way to do this is
           * with something like SDL_MapRGBA. */
          pos[0] = c.ib();
          pos[1] = c.ig();
          pos[2] = c.ir();
          pos[3] = c.ia();
        }
        void operator=(const boost::tuple<uint8, uint8, uint8>& c) {
          /** \bug Hard-coded colour masking; the correct way to do this is
           * with something like SDL_MapRGBA. */
          pos[0] = c.get<2>();
          pos[1] = c.get<1>();
          pos[2] = c.get<0>();
          pos[3] = 255;
        }
    };
  protected:
    Surface(uint16 width, uint16 height);
    virtual ~Surface() = 0;

    uint8* buffer;
    uint16 width;
    uint16 height;
  public:
    uint16 getWidth() const { return width; }
    uint16 getHeight() const { return height; }
    SurfacePixel operator()(uint16 x, uint16 y) {
      /** \bug Hard-coded 4 bytes per pixel */
      return SurfacePixel(buffer+(y*width+x)*4);
    }
};

}}

#endif // TEDOMARI__UI__SURFACE_H

