#ifndef UI__COLOUR_H
#define UI__COLOUR_H

#include "libsakusen-global.h"

/* Opaque is 1 to align with what SDL uses */
#define ALPHA_OPAQUE_DBL 1.0

namespace tedomari {
namespace ui {

class Colour {
  public:
    Colour() : r(0.0), g(0.0), b(0.0), a(ALPHA_OPAQUE_DBL) {}
    /*Colour(uint8 red, uint8 green, uint8 blue) :
      r(double(red)/255), g(double(green)/255), b(double(blue)/255),
      a(ALPHA_OPAQUE_DBL)
    {}*/
    Colour(double red, double green, double blue) :
      r(red), g(green), b(blue), a(ALPHA_OPAQUE_DBL)
    {}
    Colour(double red, double green, double blue, double alpha) :
      r(red), g(green), b(blue), a(alpha)
    {}
  private:
    double r;
    double g;
    double b;
    double a;
  public:
    inline double dr() const { return r; }
    inline double dg() const { return g; }
    inline double db() const { return b; }
    inline double da() const { return a; }

    inline uint8 ir() const { return uint8(r*255); }
    inline uint8 ig() const { return uint8(g*255); }
    inline uint8 ib() const { return uint8(b*255); }
    inline uint8 ia() const { return uint8(a*255); }
    
    /* Some predefined colours */
    static const Colour black;
    static const Colour blue;
    static const Colour green;
    static const Colour cyan;
    static const Colour red;
    static const Colour magenta;
    static const Colour yellow;
    static const Colour white;
};

}}

#endif // UI__COLOUR_H

