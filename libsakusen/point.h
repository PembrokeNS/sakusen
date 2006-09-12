#ifndef POINT_H
#define POINT_H

#include "libsakusen-global.h"
#include "typemunger.h"
#include <limits>

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
    T x;
    T y;
    T z;

    Point(): x(0), y(0), z(0) {}
    Point(T a, T b, T c): x(a), y(b), z(c) {}
    template <typename U>
    Point(const Point<U>& p) : x(T(p.x)), y(T(p.y)), z(T(p.z)) {}
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

    /** Order operators use the product order.  Note that this is a partial
     * order only, not a total order (so don't use Point<T>s in an
     * ordered container) */
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

    /* arithmetic operators */
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

    inline Point<T> operator/(const T scale) const {
      return Point<T>(x/scale, y/scale, z/scale);
    }

#if 0
    inline uint64 squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return uint64(sint64(x)*x) + uint64(sint64(y)*y) + uint64(sint64(z)*z);
    }
#endif

    typedef typename TypeMunger<T>::widest Wide;
    typedef typename TypeMunger<Wide>::unsign UWide;

    inline UWide squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return UWide(Wide(x)*x) + Wide(y)*y + Wide(z)*z;
    }

    inline double length(void) const {
      return sqrt(static_cast<double>(squareLength()));
    }

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

#ifndef SWIG
/** \brief Truncate a Point towards zero.
 *
 * This function is only defined for Point<double> and truncates each
 * co-ordinate towards zero, so you can cast it to a Point of integer type
 * knowing it is at least as close to the origin as the original.
 *
 * Don't forget that not all integral doubles have integer representations.
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
 * This function is only defined for Point<double> and rounds each co-ordinate
 * to the nearest integer, so you can cast it to a Point of integer type
 * knowing it is as close as you can get to the original.
 *
 * Don't forget that not all integral doubles have integer representations.
 */
template<>
inline Point<double> Point<double>::round(void) const {
  return Point<double>(
      trunc(x),
      trunc(y),
      trunc(z)
  );
}
#endif

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Point<T>& p) {
  out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return out;
}

#ifdef _MSC_VER
template class LIBSAKUSEN_API Point<sint16>;
template class LIBSAKUSEN_API Point<sint32>;
#endif

}

#endif // POINT_H

