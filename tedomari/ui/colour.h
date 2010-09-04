#ifndef TEDOMARI__UI__COLOUR_H
#define TEDOMARI__UI__COLOUR_H

#include <sakusen/libsakusen-global.h>

/* Opaque is 1 to align with what SDL uses */
#define TEDOMARI_UI_ALPHA_OPAQUE_DBL 1.0

namespace tedomari {
namespace ui {

/** \brief Represents a colour with alpha channel
 *
 * A colour is stored as red, green, blue and alpha components, all doubles in
 * the range [0,1].  For the alpha channel, 1 is opaque and 0 is transparent
 * (to align with SDL).
 *
 * Static members allow many common colours to be fetched without having to
 * remember the appropriate RGB values. */
class Colour {
  public:
    /** \brief Default constructor
     *
     * Creates an opaque, black Colour */
    Colour() : r(0.0), g(0.0), b(0.0), a(TEDOMARI_UI_ALPHA_OPAQUE_DBL) {}
    /** \brief uint8 opaque constructor
     *
     * Creates an opaque Colour with given RGB values (uint8 values are
     * scaled linearly from [0,255] to [0,1]). */
    Colour(uint8 red, uint8 green, uint8 blue) :
      r(double(red)/255), g(double(green)/255), b(double(blue)/255),
      a(TEDOMARI_UI_ALPHA_OPAQUE_DBL)
    {}
    /** \brief double opaque constructor
     *
     * Creates an opaque Colour with given RGB values. All arguments should be
     * in the range [0,1]. */
    Colour(double red, double green, double blue) :
      r(red), g(green), b(blue), a(TEDOMARI_UI_ALPHA_OPAQUE_DBL)
    {}
    /** \brief double constructor
     *
     * Creates a Colour with given RGBA values. All arguments should be
     * in the range [0,1]. */
    Colour(double red, double green, double blue, double alpha) :
      r(red), g(green), b(blue), a(alpha)
    {}
  private:
    double r;
    double g;
    double b;
    double a;
  public:
    /** \name double accessors
     *
     * These accessors return the components of the Colour as doubles in the
     * range [0,1]. */
    //@{
    inline double dr() const { return r; }
    inline double dg() const { return g; }
    inline double db() const { return b; }
    inline double da() const { return a; }
    inline double& dr() { return r; }
    inline double& dg() { return g; }
    inline double& db() { return b; }
    inline double& da() { return a; }
    //@}

    /** \name uint8 accessors
     *
     * These accessors return the components of the Colour as uint8s in the
     * range [0,255], scaled linearly from the internal double values. */
    //@{
    inline uint8 ir() const { return uint8(r*255); }
    inline uint8 ig() const { return uint8(g*255); }
    inline uint8 ib() const { return uint8(b*255); }
    inline uint8 ia() const { return uint8(a*255); }
    //@}
    
    /** \name Some predefined colours
     *
     * These static fields and methods allow common Colours to be easily
     * accessed. */
    //@{
    static const Colour black;
    static const Colour blue;
    static const Colour green;
    static const Colour cyan;
    static const Colour red;
    static const Colour magenta;
    static const Colour yellow;
    static const Colour white;

    /** \brief Gets a shade of grey
     *
     * \param g The common value to use for the RGB components of the Colour */
    static Colour grey(double g) { return Colour(g, g, g); }
    //@}
};

/** \brief Operator multiplying a Colour by a double
 *
 * Can be used, for example to turn red into dark red by multiplying by some
 * value in (0,1)
 */
inline Colour operator*(const Colour& c, const double d) {
  return Colour(c.dr()*d, c.dg()*d, c.db()*d);
}

}}

#endif // TEDOMARI__UI__COLOUR_H

