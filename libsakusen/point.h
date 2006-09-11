#ifndef POINT_H
#define POINT_H

#include "libsakusen-global.h"
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

    inline Point<double> operator*(const double scale) const;

    template <typename U>
    inline void operator*=(const U right) {
      x *= right;
      y *= right;
      z *= right;
    }

    inline Point<double> operator/(double scale) const;

    inline Point<T> operator/(uint32 scale) const {
      Point<T> result(x/scale,y/scale,z/scale);
			return result;
    }

    inline uint64 squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return uint64(sint64(x)*x) + sint64(y)*y + sint64(z)*z;
    }

    inline double length(void) const {
      return sqrt(static_cast<double>(squareLength()));
    }

    inline bool isZero(void) const {
      return x==0 && y==0 && z==0;
    }

    inline void zero(void) {
      x=0;
      y=0;
      z=0;
    }
};

template<>
class LIBSAKUSEN_API Point<double> {
  public:
    double x;
    double y;
    double z;

    Point(): x(0), y(0), z(0) {}
    Point(double a, double b, double c): x(a), y(b), z(c) {}
    template <typename U>
    Point(const Point<U>& p) : x(p.x), y(p.y), z(p.z) {}
    ~Point() {}

    inline double& operator[](const int index) {
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

    inline const double& operator[](const int index) const {
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

    /* Order operators use the product order.  Note that this is a partial
     * order only, not a total order (so don't use Point<T>s in an
     * ordered container) */
    template <typename U>
    inline bool operator<=(const Point<U>& right) const {
      return x<=right.x && y<=right.y && z<=right.z;
    }

    template <typename U>
    inline bool operator<(const Point<U>& right) const {
      return x<right.x && y<right.y && z<right.z;
    }
    
    template <typename U>
    inline bool operator>=(const Point<U>& right) const {
      return x>=right.x && y>=right.y && z>=right.z;
    }

    template <typename U>
    inline bool operator>(const Point<U>& right) const {
      return x>right.x && y>right.y && z>right.z;
    }
    
    inline Point<double> operator-() const {
      Point <double> result(-x,-y,-z);
      return result;
    }

    template <typename U>
    inline Point<double> operator+(const Point<U>& right) const {
      Point<double> result(x+right.x,y+right.y,z+right.z);
      return result;
    }

    template <typename U>
    inline void operator+=(const Point<U>& right) {
      x += right.x;
      y += right.y;
      z += right.z;
    }

    template <typename U>
    inline Point<double> operator-(const Point<U>& right) const {
      Point<double> result(x-right.x,y-right.y,z-right.z);
      return result;
    }

    template <typename U>
    inline void operator-=(const Point<U>& right) {
      x -= right.x;
      y -= right.y;
      z -= right.z;
    }

    template<typename U>
    inline Point<double> operator*(U scale) const {
      Point<double> result(x*scale,y*scale,z*scale);
			return result;
    }

    template<typename U>
    inline Point<double> operator/(U scale) const {
      Point<double> result(x/scale,y/scale,z/scale);
			return result;
    }

    template<typename U>
    inline Point<U> truncate(void) const {
      return Point<U>(
          static_cast<U>(trunc(x)),
          static_cast<U>(trunc(y)),
          static_cast<U>(trunc(z))
        );
    }

    template<typename U>
    inline Point<U> round(void) const {
      return Point<U>(
          static_cast<U>(::round(x)),
          static_cast<U>(::round(y)),
          static_cast<U>(::round(z))
        );
    }
};

template<typename T>
inline Point<double> Point<T>::operator/(double scale) const {
  Point<double> result(x/scale,y/scale,z/scale);
	return result;
}

template<typename T>
inline Point<double> Point<T>::operator*(double scale) const {
  return Point<double>(x*scale,y*scale,z*scale);
}

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Point<T>& p) {
  out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return out;
}

#ifdef _MSC_VER
template class LIBSAKUSEN_API Point<sint16>;
#endif

}

#endif // POINT_H

