#ifndef UI__RECTANGLE_H
#define UI__RECTANGLE_H

#include "point.h"
#include "iarchive.h"
#include "oarchive.h"
#include "ref.h"

/** \file
 * A Rectangle is the region enclosed by two points in the plane. Both corners
 * (thus all edges) are inside the Rectangle. There is a unique empty
 * Rectangle, and it has width and height 0. A Rectangle with both corners
 * equal contains exactly one point in the plane.
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

  /** \brief Construct the empty Rectangle. */
  /* (yes, it should be this way around) */
  Rectangle() :
    minx(topNumber<T>()),
    miny(topNumber<T>()),
    maxx(bottomNumber<T>()),
    maxy(bottomNumber<T>()) {}

  /** \brief Construct a Rectangle with the given corners.
   *
   * It must be the case that \p x1 \<= \p x2 and \p y1 \<= \p y2. This is not
   * checked.
   *
   * \param x1,y1 Bottom-left corner
   * \param x2,y2 Top-right corner
   */
  Rectangle(T x1, T y1, T x2, T y2) :
    minx(x1), miny(y1), maxx(x2), maxy(y2) {}
  /** \brief Construct the smallest Rectangle that contains both of the given Points.
   *
   * The z co-ordinates of the Points are ignored. The order of the Points is
   * irrelevant. The resulting rectangle will have \p c1 and \p c2 as corners,
   * but not necessarily the bottom-left and top-right corners.
   */
  Rectangle(const Point<T>& c1, const Point<T>& c2) :
    minx(static_cast<T>(std::min(c1.x, c2.x))), miny(static_cast<T>(std::min(c1.y, c2.y))),
    maxx(static_cast<T>(std::max(c1.x, c2.x))), maxy(static_cast<T>(std::max(c1.y, c2.y))) {}

  /** \brief Width of the Rectangle.
   *
   * Gives the number of columns inside the Rectangle. It only returns 0 for
   * the empty Rectangle.
   */
  inline T getWidth() const {
    if (isEmpty()) return 0;
    return maxx - minx;
  }

  /** \brief Height of the Rectangle.
   *
   * Gives the number of rows inside the Rectangle. It only returns 0 for
   * the empty Rectangle.
   */
  inline T getHeight() const {
    if (isEmpty()) return 0;
    return maxy - miny;
  }

  /** Get the corners of the Rectangle. The result is undefined for the empty
   * Rectangle.
   */
  /*@{*/
  inline T getMinX() const { return minx; }
  inline T getMinY() const { return miny; }
  inline T getMaxX() const { return maxx; }
  inline T getMaxY() const { return maxy; }
  /*@}*/
  inline bool isEmpty() const { return (getMinX() > getMaxX() && getMinY() > getMaxY()); }
  inline bool contains(const Point<T>& p) const {
    return p.x >= getMinX() && p.x <= getMaxX() && p.y >= getMinY() && p.y <= getMaxY();
  }
  /** \brief Do these Rectangles intersect?
   *
   * \return true iff there exists at least one point inside both rectangles.
   */
  inline bool intersects(const Rectangle<T> r) const {
    /* don't need to check if r.isEmpty() because we only get a false match if
     * both rectangles are empty
     */
    return r.getMinX() <= getMaxX() && r.getMaxX() >= getMinX() &&
      r.getMinY() <= getMaxY() && r.getMaxY() >= getMinY() && !isEmpty();
  }
  bool fastIntersects(const ICompleteUnit*) const;
  bool fastIntersects(const Ref<const ISensorReturns>&) const;
  /** \brief Computes the intersection of intersecting Rectangles.
   *
   * If the rectangles have a non-empty intersection, returns the Rectangle of
   * their intersection. If they do not intersect, returns the empty Rectangle.
   */
  inline Rectangle<T> intersect(const Rectangle<T>& right) const {
    if (!intersects(right)) return Rectangle();
    return Rectangle<T>(
        static_cast<T>(std::max(minx, right.minx)),
        static_cast<T>(std::max(miny, right.miny)),
        static_cast<T>(std::min(maxx, right.maxx)),
        static_cast<T>(std::min(maxy, right.maxy))
      );
  }
  Rectangle<T> enclosure(const Rectangle<T>& right) const;

  bool operator==(const Rectangle<T>& right) const;
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

#ifdef LIBSAKUSEN_METHOD_DEFINITIONS
/** \brief Get the smallest Rectangle enclosing both.
 *
 * Returns the smallest rectangle that completely contains both this
 * Rectangle and the given Rectangle. If either is empty, returns a copy of
 * the other; if both are empty, returns an empty Rectangle.
 */
template<typename T>
Rectangle<T> Rectangle<T>::enclosure(const Rectangle<T>& right) const {
  return Rectangle<T>(
      static_cast<T>(std::min(minx, right.minx)),
      static_cast<T>(std::min(miny, right.miny)),
      static_cast<T>(std::max(maxx, right.maxx)),
      static_cast<T>(std::max(maxy, right.maxy))
    );
}

/** \brief Compare two Rectangles for equality.
 *
 * \return true if either or both of the following conditions is met: all four
 * points of one Rectangle are equal to the corresponding points of the other;
 * or both rectangles are empty
 */
template<typename T>
bool Rectangle<T>::operator==(const Rectangle<T>& right) const {
  return ((minx == right.minx && miny == right.miny
      && maxx == right.maxx && maxy == right.maxy));
}
#endif
}

#endif // UI__RECTANGLE_H

