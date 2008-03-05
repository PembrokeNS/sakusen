#ifndef POINT_H
#define POINT_H

#include "libsakusen-global.h"
#include <boost/utility.hpp>

#include "intmunger.h"

namespace sakusen {

/** \brief Get the most negative number.
 * \return The minimal number of type T
 *
 * \see topNumber()
 * \note This is an inline function whose result can always be computed at
 * compile-time, so if your compiler is at all on the ball it should reduce to
 * including a constant.
 *
 * \bug This fails to handle the case where a floating-point format for the
 * template T doesn't have infinity, is signed, and has the most -ve
 * representable number not equal to the negation of the most +ve
 * representable number. So sue me.
 */

/* Apparently MSVC is on the ball in this case, but because it has reduced the 
 * answer to a constant, generates a warning (4101) about the now unreferenced local variable.
 * This is slightly mystifying for a few seconds, and clutters things up, so I have disabled
 * it.
 * 4146 is a warning telling me that when I apply a unary minus to an unsigned operator, 
 * nothing happens. Thanks MSVC.
 */
#ifdef _MSC_VER 
	#pragma warning (disable:4101 4146)
#endif

template<typename T>
inline T bottomNumber() {
  std::numeric_limits<T> limits;
  
  if (limits.is_integer)
    return limits.min();
  if (limits.is_signed) {
    if (limits.has_infinity)
      return -(limits.infinity());
    else
      return -(limits.max());
  } else
    return static_cast<T>(0);
}

/** \brief Get the most positive number.
 * \return The maximal number of type T
 *
 * \see bottomNumber()
 * \note This is an inline function whose result can always be computed at
 * compile-time, so if your compiler is at all on the ball it should reduce to
 * including a constant.
 */

template<typename T>
inline T topNumber() {
  std::numeric_limits<T> limits;

  if (limits.has_infinity)
    return limits.infinity();
  else
    return limits.max();
}
/** \brief point in 3-space
 *
 * Point represents a vector in some three-dimensional space. This vector could
 * represent a position, a direction, an acceleration, or something else.
 *
 * Positions and directions in game-space are of type Point<sint32>.
 */
template<typename T>
class LIBSAKUSEN_API Point {
  public:
    typedef T element_type;

    T x;
    T y;
    T z;

    Point(): x(0), y(0), z(0) {}
    Point(T a, T b, T c): x(a), y(b), z(c) {}
    /** \brief Simple copy constructor for the sake of swig's sanity */
    Point(const Point<T>& p) : x(p.x), y(p.y), z(p.z) {}
#ifndef SWIG
    /** \brief Implicit copy constructor for when type U casts to type T
     * without possibility of loss */
    template <typename U>
    Point(
        const Point<U>& p,
        typename boost::enable_if<typename LosslessCast<T,U>::type,int>::type=0
      ) : x(T(p.x)), y(T(p.y)), z(T(p.z)) {}
    /** \brief Explicit copy constructor for when casting type U to type T
     * may incur loss */
    template <typename U>
    explicit Point(
        const Point<U>& p,
        typename boost::disable_if<typename LosslessCast<T,U>::type,int>::type=0
      ) : x(T(p.x)), y(T(p.y)), z(T(p.z)) {}
#endif /*SWIG*/
    ~Point() {}

    /** \brief Construct the bottom-left-most Point.
     *
     * Returns an instance of the Point in the bottom-left corner of space.
     * This point is minimal in the partial ordering on Points, and all three
     * co-ordinates are the minimum representable number.
     */
    inline static Point<T> bottom() {
      T coord = bottomNumber<T>();
      return Point(coord, coord, coord);
    }

    /** \brief Construct the top-right-most Point.
     *
     * Returns an instance of the Point in the top-right corner of space.
     * This point is maximal in the partial ordering on Points, and all three
     * co-ordinates are the maximum representable number.
     */
    inline static Point<T> top() {
      T coord = topNumber<T>();
      return Point(coord, coord, coord);
    }

    inline T& operator[](const int index) {
      switch (index) {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          Fatal("index " << index << " out of bounds");
      }
    }

    inline const T& operator[](const int index) const {
      switch (index) {
        case 0:
          return x;
        case 1:
          return y;
        case 2:
          return z;
        default:
          Fatal("index " << index << " out of bounds");
      }
    }

    template <typename U>
    inline bool operator==(const Point<U>& right) const {
      return x==right.x && y==right.y && z==right.z;
    }

    template <typename U>
    inline bool operator!=(const Point<U>& right) const {
      return x!=right.x || y!=right.y || z!=right.z;
    }

    /** \name Order operators
     *
     * \note These use the product order.  This is a partial
     * order only, not a total order (so don't use Point<T>s in an
     * ordered container) */
    //@{
    template <typename U>
    inline bool operator<=(const Point<U>& right) const {
      return x<=right.x && y<=right.y && z<=right.z;
    }

    template <typename U>
    inline bool operator<(const Point<U>& right) const {
      return (*this <= right) && (*this != right);
    }
    
    template <typename U>
    inline bool operator>=(const Point<U>& right) const {
      return x>=right.x && y>=right.y && z>=right.z;
    }

    template <typename U>
    inline bool operator>(const Point<U>& right) const {
      return (*this >= right) && (*this != right);
    }
    //@}

    /** \name Arithmetic operators */
    //@{
    inline Point<T> operator-() const {
      Point <T> result(-x,-y,-z);
      return result;
    }

    template <typename U>
    inline Point<T> operator+(const Point<U>& right) const {
      Point<T> result(x+right.x,y+right.y,z+right.z);
      return result;
    }

    template <typename U>
    inline void operator+=(const Point<U>& right) {
      x += right.x;
      y += right.y;
      z += right.z;
    }

    template <typename U>
    inline Point<T> operator-(const Point<U>& right) const {
      Point<T> result(x-right.x,y-right.y,z-right.z);
      return result;
    }

    template <typename U>
    inline void operator-=(const Point<U>& right) {
      x -= right.x;
      y -= right.y;
      z -= right.z;
    }

    inline Point<T> operator*(const T scale) const {
      return Point<T>(x*scale, y*scale, z*scale);
    }

    template <typename U>
    inline void operator*=(const U right) {
      x *= right;
      y *= right;
      z *= right;
    }

    inline Point<T>& operator/=(const T scale) {
      x /= scale; y /= scale; z /= scale;
      return *this;
    }

    inline Point<T> operator/(const T scale) const {
      return Point<T>(*this) /= scale;
    }
    //@}
    
    /** \brief Return the cross product of this Point with another. */
    inline Point<T> crossProduct(const Point<T>& right) {
      return Point<T>(
          y*right.z-z*right.y,
          z*right.x-x*right.z,
          x*right.y-y*right.x
        );
    }

    /* Typedefs to stop squareLength() becoming horrible, with accompanying
     * #defines because the typedefs are far too complex for SWIG's tiny mind.
     */
#ifdef SWIG
#define Wide IntMunger<T>::widest
#define UWide IntMunger<IntMunger<T>::widest>::unsign
#else
  private:
    typedef typename IntMunger<T>::widest Wide;
    typedef typename IntMunger<Wide>::unsign UWide;
  public:
#endif

    inline UWide squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return UWide(UWide(Wide(x)*x) + Wide(y)*y + Wide(z)*z);
    }

    inline UWide innerProduct(const Point<T>& right) const {
      return UWide(UWide(Wide(x)*right.x) + Wide(y)*right.y + Wide(z)*right.z);
    }

    inline double length(void) const {
      return sqrt(static_cast<double>(squareLength()));
    }

    /** \brief Rescale the Point to length 1.
     *
     * This is only useful on Point<double>s.
     *
     * Unfortunately, on MSVC it causes (many) compile warnings for the other
     * types when the library is compiled, due to template instantiation.
     * Thus we have separate implementations for double and not double.
     * The double version is below. */
    inline void normalise(void) {
      Fatal("normalise on Point<T> for T!=double");
    };

    inline bool isZero(void) const {
      return x==T(0) && y==T(0) && z==T(0);
    }

    inline void zero(void) {
      x=T(0);
      y=T(0);
      z=T(0);
    }

#ifndef SWIG
    inline Point<T> truncate(void) const;
    inline Point<T> round(void) const;
#endif
};

template<>
inline void Point<double>::normalise(void) {
  *this /= length();
}

#ifndef SWIG
/** \brief Truncate a Point towards zero.
 *
 * This function is defined for all types. For Point<double> it truncates each
 * co-ordinate towards zero, so you can cast it to a Point of integer type
 * knowing it is at least as close to the origin as the original.
 *
 * Don't forget that not all integral doubles have integer representations.
 *
 * For all other types, it has no effect, and returns the Point.
 */
template<>
inline Point<double> Point<double>::truncate(void) const {
  return Point<double>(
      trunc(x),
      trunc(y),
      trunc(z)
  );
}

/** \brief Round a Point to the nearest integer co-ordinates.
 *
 * This function is defined for all types. For Point<double> it rounds each co-ordinate
 * to the nearest integer, so you can cast it to a Point of integer type
 * knowing it is as close as you can get to the original.
 *
 * Don't forget that not all integral doubles have integer representations.
 *
 * For all other types, it has no effect, and returns the Point.
 */
template<>
inline Point<double> Point<double>::round(void) const {
  return Point<double>(
      trunc(x),
      trunc(y),
      trunc(z)
  );
}

#if defined LIBSAKUSEN_METHOD_DEFINITIONS

template<typename T>
inline Point<T> Point<T>::truncate(void) const {
  return Point<T>(x,y,z);
}

template<typename T>
inline Point<T> Point<T>::round(void) const {
  return Point<T>(x,y,z);
}

#endif //LIBSAKUSEN_METHOD_DEFINITIONS
#endif //! SWIG

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Point<T>& p) {
  out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return out;
}

/** \name Typedefs for some common uses of Point, to make it clear what a Point
 * is representing in any given context */
//@{
typedef Point<sint16> Acceleration;
typedef Point<sint16> Velocity;
typedef Point<sint32> Position;
//@}

#ifdef _MSC_VER
template class LIBSAKUSEN_API Point<sint16>;
template class LIBSAKUSEN_API Point<uint16>;
template class LIBSAKUSEN_API Point<sint32>;
template class LIBSAKUSEN_API Point<uint32>;
template class LIBSAKUSEN_API Point<sint64>;
template class LIBSAKUSEN_API Point<double>;
#endif
}

#ifdef _MSC_VER //Re-enable warnings disabled above, in case it is useful later.
#pragma warning (default:4101 4146)
#endif

#endif // POINT_H

