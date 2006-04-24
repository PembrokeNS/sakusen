#ifndef UI__RECTANGLE_H
#define UI__RECTANGLE_H

#include "point.h"
#include "iarchive.h"
#include "oarchive.h"

/** \file
 * A ::Rectangle is the region enclosed by two points in the plane. The
 * bottom-left point is inside the rectangle but the top-right point is
 * outside. This means you can't have a rectangle which all points are inside
 * but you can have a rectangle with zero area.
 *
 * \todo What happens when you have a Rectangle<sint32> whose points are
 * greater than 2^31 apart and you take the width/height?
 */
namespace sakusen {

class ICompleteUnit;
class ISensorReturns;

template<typename T>
struct LIBSAKUSEN_API Rectangle {
  /* fields */
  T minx; /**< The minimum value of x */
  T miny; /**< The minimum value of y */
  T maxx; /**< The maximum value of x */
  T maxy; /**< The maximum value of y */

  /* methods */
  Rectangle() : minx(0), miny(0), maxx(0), maxy(0) {}
  /** Constructor, does the obvious thing. It must be the case that \p x1 \< \p
   * x2 and \p y1 \< \p y2.
   *
   * \param x1,y1 Bottom-left corner
   * \param x2,y2 Top-right corner
   */
  Rectangle(T x1, T y1, T x2, T y2) :
    minx(x1), miny(y1), maxx(x2), maxy(y2) {}
  Rectangle(const Point<T>& c1, const Point<T>& c2) :
    minx((T)std::min(c1.x, c2.x)), miny((T)std::min(c1.y, c2.y)),
    maxx((T)std::max(c1.x, c2.x)), maxy((T)std::max(c1.y, c2.y)) {}

  inline T getWidth() const { return maxx - minx; }
  inline T getHeight() const { return maxy - miny; }
  inline T getMinX() const { return minx; }
  inline T getMinY() const { return miny; }
  inline T getMaxX() const { return maxx; }
  inline T getMaxY() const { return maxy; }
  inline bool contains(const Point<T>& p) const {
    return p.x >= minx && p.x < getMaxX() && p.y >= miny && p.y < getMaxY();
  }
  inline bool intersects(const Rectangle<T> r) const {
    return r.minx < getMaxX() && r.getMaxX() > minx &&
      r.miny < getMaxY() && r.getMaxY() > miny;
  }
  bool fastIntersects(const ICompleteUnit*) const;
  bool fastIntersects(const ISensorReturns*) const;

  void store(OArchive& out) const {
    out << minx << miny << maxx << maxy;
  }
  static Rectangle<T> load(IArchive& in) {
    T minx;
    T miny;
    T maxx;
    T maxy;
    in >> minx >> miny >> maxx >> maxy;
    return Rectangle<T>(minx, miny, maxx, maxy);
  }
};

}

#endif // UI__RECTANGLE_H

