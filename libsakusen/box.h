#ifndef BOX_H
#define BOX_H

#include "libsakusen-global.h"
#include "point.h"

namespace sakusen {

/** Represents an axis-aligned box (cuboid) in 3D space.
 *
 * A box is defined by its two corner points x1 and x2, where x1 \<= x2.
 * A point p is inside the box if forall basis vectors e, x1_e \<= p_e
 * \< x2_e. The template T is passed on to the ::Point.
 *
 * The empty box has x1 = x2.
 */
template<typename T>
class LIBSAKUSEN_API Box {
  protected:
    Point<T> min;
    Point<T> max;
  public:
    /** Uses the default Point twice to create an empty ::Box. */
    Box(): min(), max() {}
    /** Creates a Box with the given corners. The caller is
     * responsible for ensuring that \p x1 \<= \p x2.
     */
    Box(Point<T> x1, Point<T> x2): min(x1), max(x2) {}
    Box(const Box<T>& b): min(b.min), max(b.max) {}
    ~Box() {}
    /** For a Box \c b, \c b[0] is the lower-left corner, and \c b[1]
     * is the upper-right corner.
     */
    inline T& operator[](const int i) const {
      switch (i) {
        case 0:
          return min;
        case 1:
          return max;
        default:
          Fatal("index " << i << " out of bounds");
      }
    }

    inline const Point<T>& getMin() const {
      return min;
    }

    inline const Point<T>& getMax() const {
      return max;
    }

    inline bool operator==(const Box b) const {
      return min == b.min && max == b.max;
    }

    inline std::ostream& operator<<(std::ostream& out) {
      out << "box <" << min << "," << max << ">";
      return out;
    }

    /** Tests whether \c this is a valid Box. */
    inline bool isValid() const {
      return (min <= max);
    }

    /** \brief Tests whether this Box is empty (i.e. there does not exist
     * a Point inside it).
     * 
     * In fact, it just compares the lower-left point and the upper-right
     * point for equality.
     */
    inline bool isEmpty() const {
      return (min == max);
    }
    bool contains(const Point<T>& p) const;
    bool operator<=(const Box<T>& b) const;
    bool intersects(const Box<T>& b) const;
    Box<T> intersection(const Box<T>& b) const;
    Box<T> enclosure(const Box<T>& b) const;

    /** \brief Gets the smallest Rectangle bounding this Box */
    Rectangle<T> rectangle() const {
      return Rectangle<T>(min.x, min.y, max.x, max.y);
    }
};

/** \brief Tests whether \p p is inside this ::Box.
 *
 * The lower-left boundary of the ::Box is inside but the upper-right
 * boundary is outside.
 */
template<typename T>
bool Box<T>::contains(const Point<T>& p) const {
  return (p.x >= min.x && p.y >= min.y && p.z >= min.z &&
    p.x < max.x && p.y < max.y && p.z < max.z);
}

/** \brief Tests whether \p b is entirely inside this ::Box.
 *
 * Again, the lower-left boundary is inside. The upper-right boundary
 * is outside, but since the upper-right boundary is also outside \p b,
 * the comparison is non-strict at both ends. This is a partial order.
 */
template<typename T>
bool Box<T>::operator<=(const Box<T>& b) const {
  return (min.x <= b.min.x && min.y <= b.min.y && min.z <= b.min.z &&
      max.x >= b.max.x && max.y >= b.max.y && max.z >= b.max.z);
}

/** \brief Tests whether any point inside \p b is also inside this
 * ::Box.
 *
 * This relation is symmetric.
 */
template<typename T>
bool Box<T>::intersects(const Box<T>& b) const {
  return (min < b.max && max > b.min);
}

/** \brief Returns the ::Box containing all the points inside both this
 * and \p b.
 *
 * If this does not intersect \p b, then the result will be empty, but
 * it is not defined \b which empty ::Box it will be.
 * 
 * \todo Use fewer branches.
 */
template<typename T>
Box<T> Box<T>::intersection(const Box<T>& b) const {
  Box i(this);
  if (intersects(b)) {
    if (min.x < b.min.x) i.min.x = b.min.x;
    if (min.y < b.min.y) i.min.y = b.min.y;
    if (min.z < b.min.z) i.min.z = b.min.z;

    if (max.x > b.max.x) i.max.x = b.max.x;
    if (max.y > b.max.y) i.max.y = b.max.y;
    if (max.z > b.max.z) i.max.z = b.max.z;
  } else {
    i.max = i.min;
  }
  return i;
}

/** \brief Unions \p b with this one and returns the result.
 *
 * This returns the smallest Box that contains both this one and \p b.
 * Unless the boxes are neatly aligned, the result will also contain
 * lots of points outside the two, but you can't help that.
 *
 * If both boxes are empty, the result will not necessarily be empty. It
 * will be the minimal Box that contains the corners of both. If that
 * annoys you, check for emptiness first.
 *
 * \todo Use fewer branches.
 */
template<typename T>
Box<T> Box<T>::enclosure(const Box<T>& b) const {
  Box u(this);
  if (min.x > b.min.x) u.min.x = b.min.x;
  if (min.y > b.min.y) u.min.y = b.min.y;
  if (min.z > b.min.z) u.min.z = b.min.z;

  if (max.x < b.max.x) u.max.x = b.max.x;
  if (max.y < b.max.y) u.max.y = b.max.y;
  if (max.z < b.max.z) u.max.z = b.max.z;

  return u;
}

}
#endif
