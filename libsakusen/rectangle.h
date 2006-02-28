#ifndef UI__RECTANGLE_H
#define UI__RECTANGLE_H

#include "point.h"

namespace sakusen {

class ICompleteUnit;

template<typename T>
struct Rectangle {
  Rectangle() : x(0), y(0), width(0), height(0) {}
  Rectangle(T xoffset, T yoffset, T w, T h) :
    x(xoffset), y(yoffset), width(w), height(h) {}
  T x; /**< The minimum value of x */
  T y; /**< The minimum value of y */
  T width;
  T height;

  inline T getMaxX() const { return x+width; }
  inline T getMaxY() const { return y+height; }
  inline bool contains(const Point<T>& p) const {
    return p.x >= x && p.x < getMaxX() && p.y >= y && p.y < getMaxY();
  }
  bool intersects(const Rectangle<T> r) const {
    return r.x < getMaxX() && r.getMaxX() > x &&
      r.y < getMaxY() && r.getMaxY() > y;
  }
  bool fastIntersects(const ICompleteUnit*) const;
};

}

#endif // UI__RECTANGLE_H

