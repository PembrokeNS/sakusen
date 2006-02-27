#ifndef POINT_H
#define POINT_H

#include "libsakusen-global.h"

namespace sakusen {

template <typename T> class LIBSAKUSEN_API Point {
  public:
    T x;
    T y;
    T z;

    Point(): x(0), y(0), z(0) {}
    Point(T a, T b, T c): x(a), y(b), z(c) {}
    template <typename U>
    Point(const Point<U>& p) : x(p.x), y(p.y), z(p.z) {}
    ~Point() {}

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
          Fatal("index " << index << "out of bounds");
      }
    }

    template <typename U>
    inline bool operator==(const Point<U> right) const {
      return x==right.x && y==right.y && z==right.z;
    }

    /* Order operators use the product order.  Note that this is a partial
     * order only, not a total order (so don't use Point<T>s in an
     * ordered container) */
    template <typename U>
    inline bool operator<=(const Point<U> right) const {
      return x<=right.x && y<=right.y && z<=right.z;
    }

    template <typename U>
    inline bool operator<(const Point<U> right) const {
      return x<right.x && y<right.y && z<right.z;
    }
    
    template <typename U>
    inline bool operator>=(const Point<U> right) const {
      return x>=right.x && y>=right.y && z>=right.z;
    }

    template <typename U>
    inline bool operator>(const Point<U> right) const {
      return x>right.x && y>right.y && z>right.z;
    }
    
    inline Point<T> operator-() const {
      Point <T> result(-x,-y,-z);
      return result;
    }

    template <typename U>
    inline Point<T> operator+(const Point<U> right) const {
      Point<T> result(x+right.x,y+right.y,z+right.z);
      return result;
    }

    template <typename U>
    inline void operator+=(const Point<U> right) {
      x += right.x;
      y += right.y;
      z += right.z;
    }

    template <typename U>
    inline Point<T> operator-(const Point<U> right) const {
      Point<T> result(x-right.x,y-right.y,z-right.z);
      return result;
    }

    template <typename U>
    inline void operator-=(const Point<U> right) {
      x -= right.x;
      y -= right.y;
      z -= right.z;
    }

    inline Point<double> operator*(const double scale) const {
      return Point<double>(x*scale,y*scale,z*scale);
    }

    template <typename U>
    inline void operator*=(const U right) {
      x *= right;
      y *= right;
      z *= right;
    }

    inline Point<double> operator/(double scale) const {
      Point<double> result(x/scale,y/scale,z/scale);
			return result;
    }

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

    inline Point<sint16> truncate16(void) const;
    inline Point<sint32> truncate32(void) const;

    inline Point<sint16> round16(void) const;
    inline Point<sint32> round32(void) const;
};

template<>
inline Point<sint16> Point<double>::truncate16(void) const {
  return Point<sint16>(
      static_cast<sint16>(trunc(x)),
      static_cast<sint16>(trunc(y)),
      static_cast<sint16>(trunc(z))
    );
}

template<>
inline Point<sint32> Point<double>::truncate32(void) const {
  return Point<sint32>(
      static_cast<sint32>(trunc(x)),
      static_cast<sint32>(trunc(y)),
      static_cast<sint32>(trunc(z))
    );
}

template<>
inline Point<sint16> Point<double>::round16(void) const {
  return Point<sint16>(
      static_cast<sint16>(round(x)),
      static_cast<sint16>(round(y)),
      static_cast<sint16>(round(z))
    );
}

template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Point<T>& p) {
  out << "(" << p.x << ", " << p.y << ", " << p.z << ")";
  return out;
}

}

#endif // POINT_H

