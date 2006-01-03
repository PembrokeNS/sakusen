#ifndef VECTORS_H
#define VECTORS_H

#include "libsakusen-global.h"
/* VC++ gives a (couple hundred) warning(s) about unary minus operators being applied to unsigned types here.
This links back to the definition of the '-' operator in this file, which of course does nothing when applied 
to Point<uintX>. John does not belive he has used such a thing, so it is probably just an artifact of explicit 
template instantiation. To aid reading of build output I #pragma it away here. 

  I remind everyone that expecting -Point<uintX> to be different from Point<uintX> will not be renumerative.
*/

#if defined(_MSC_VER)
#pragma warning(push)
//#pragma warning(disable: 4146)
//#pragma warning(disable: 4244)
#endif

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
    inline uint64 squareLength(void) const {
      /* Ugly casting here because of paranoia. */
      return uint64(sint64(x)*x) + sint64(y)*y + sint64(z)*z;
    }

#if defined(_MSC_VER)
/* Nasty HACK: Apparently VC6 has no implementation of the conversion from unsigned __int64 to double. 
Hence for VC6 we use sint64. It is fairly unlikely that we shall ever have maps this large anyway, but we should fix it later. */
    inline double length(void) const {
      return sqrt(static_cast<double>(sint64(squareLength())));
    }
#else
    inline double length(void) const {
      return sqrt(static_cast<double>(squareLength()));
    }
#endif //MSC_VER

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

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#ifdef NEED_TEMPLATE_INSTANTIATION
#pragma warning (disable: 4231)
/* I barely know what I'm doing here - JJB */
/*I also barely know what I am doing */

template Point<float> Point<uint32>::truncate16() const;
template Point<float> Point<sint32>::truncate16() const;
template Point<float> Point<uint16>::truncate16() const;
template Point<float> Point<sint16>::truncate16() const;
/*
EXPORT(Point<uint32>)
EXPORT(Point<sint32>)
EXPORT(Point<uint16>)
EXPORT(Point<sint16>)
*/
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
