#ifndef VECTORS_H
#define VECTORS_H

#include "libsakusen-global.h"

template <typename T>
class Point {
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
/* Nasty HACK: Apparently VC6 has no implementation of the conversion from unsigned __int64 to double. 
Hence for VC6 we use sint64. It is fairly unlikely that we shall ever have maps this large anyway, but we should fix it later. */
#if defined(_MSC_VER)
	inline sint64 squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return sint64(x)*x + sint64(y)*y + sint64(z)*z;
    }
#else
	inline uint64 squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return uint64(sint64(x)*x) + sint64(y)*y + sint64(z)*z;
    }
#endif //MSC_VER

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

#ifdef NEED_TEMPLATE_INSTANTIATION
#pragma warning (disable: 4231)
/* I barely know what I'm doing here - JJB */
extern template Point<uint32>::truncate16() const;
extern template Point<sint32>::truncate16() const;
extern template Point<uint16>::truncate16() const;
extern template Point<sint16>::truncate16() const;

/*EXPORT(Point<uint32>)
EXPORT(Point<sint32>)
EXPORT(Point<uint16>)
EXPORT(Point<sint16>)*/
#endif

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

#endif // VECTORS_H
