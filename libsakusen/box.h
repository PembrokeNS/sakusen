#ifndef BOX_H
#define BOX_H

#include "libsakusen-global.h"
#include "point.h"
#include "rectangle.h"

namespace sakusen {

/** \brief Represents an axis-aligned box (cuboid) in 3D space.
 *
 * A box is defined by its two corner points x1 and x2, where x1 \<= x2.
 * A point p is inside the box if forall basis vectors e, x1_e \<= p_e
 * \<= x2_e. The template T is passed on to the ::Point.
 *
 * There is a unique empty Box.
 */
template<typename T>
class LIBSAKUSEN_API Box {
  protected:
    Point<T> min;
    Point<T> max;
  public:
    /** Constructs an empty Box. */
    Box(): min(Point<T>::top()), max(Point<T>::bottom()) {}
    /** Creates a Box with the given corners. The caller is
     * responsible for ensuring that \p x1 \<= \p x2.
     */
    Box(const Point<T>& x1, const Point<T>& x2): min(x1), max(x2) {}
    /** Constructs a box whose x-y extents are those of the given rectangle,
     * and whose z extent is as specified (defaulting to the whole range) */
    Box(
        const Rectangle<T>& xy,
        T minz = bottomNumber<T>(),
        T maxz = topNumber<T>()
      ) :
      min(Point<T>(xy.minx, xy.miny, minz)),
      max(Point<T>(xy.maxx, xy.maxy, maxz))
    {}
    Box(const Box<T>& b): min(b.min), max(b.max) {}
    ~Box() {}
    /** For a Box \c b, \c b[0] is the lower-left corner, and \c b[1]
     * is the upper-right corner.
     */
    inline const Point<T>& operator[](const int i) const {
      switch (i) {
        case 0:
          return min;
        case 1:
          return max;
        default:
          Fatal("index " << i << " out of bounds");
      }
    }

    /** \brief Get the back-bottom-left corner of a Box.
     *
     * The result is undefined if the Box is empty.
     */
    inline const Point<T>& getMin() const {
      return min;
    }

    /** \brief Get the front-top-right corner of a Box.
     *
     * The result is undefined if the Box is empty.
     */
    inline const Point<T>& getMax() const {
      return max;
    }

    /** Two Boxes are equal iff both of their points are equal or they are both
     * empty.
     */
    inline bool operator==(const Box b) const {
      return min == b.min && max == b.max;
    }

    /** \brief Tests whether this Box is empty (i.e. there does not exist
     * a Point inside it).
     */
    inline bool isEmpty() const {
      return (min > max);
    }

    bool contains(const Point<T>& p) const;
    bool operator<=(const Box<T>& b) const;
    bool intersects(const Box<T>& b) const;
    Box<T> intersection(const Box<T>& b) const;
    Box<T> enclosure(const Box<T>& b) const;
    Rectangle<T> rectangle() const;
    
};

/** Prints the co-ordinates of a box, for debugging purposes. */
template<typename T>
inline std::ostream& operator<<(std::ostream& out, const Box<T>& box) {
  out << "box <" << box.getMin() << "," << box.getMax() << ">";
  return out;
}

#ifdef _MSC_VER
template class LIBSAKUSEN_API Box<sint32>;
#endif

#ifdef LIBSAKUSEN_METHOD_DEFINITIONS

/** \brief Tests whether \p p is inside this ::Box.
 */
template<typename T>
bool Box<T>::contains(const Point<T>& p) const {
  return (p.x >= min.x && p.y >= min.y && p.z >= min.z &&
    p.x <= max.x && p.y <= max.y && p.z <= max.z);
}

/** \brief Gets the smallest Rectangle bounding this Box.
     *
     * Obviously, for the empty Box it returns the empty Rectangle.
     */
template<typename T> Rectangle<T> Box<T>::rectangle() const {
      return Rectangle<T>(min.x, min.y, max.x, max.y);
    }


/** \brief Tests whether \p b is entirely inside this ::Box.
 */
template<typename T>
bool Box<T>::operator<=(const Box<T>& b) const {
  return (min.x <= b.min.x && min.y <= b.min.y && min.z <= b.min.z &&
      max.x >= b.max.x && max.y >= b.max.y && max.z >= b.max.z);
}

/** \brief Tests whether two boxes intersect.
 *
 * \return true iff there exists a point that is inside both boxes.
 *
 * This relation is symmetric.
 */
template<typename T>
bool Box<T>::intersects(const Box<T>& b) const {
  return (min <= b.max && max >= b.min && !isEmpty());
}

/** \brief Returns the ::Box containing all the points inside both this
 * and \p b.
 *
 * If this does not intersect \p b, then the result will be empty, but
 * it is not defined \b which empty ::Box it will be. This method commutes.
 * 
 * \todo Use fewer branches.
 */
template<typename T>
Box<T> Box<T>::intersection(const Box<T>& b) const {
  Box i(*this);
  if (intersects(b)) {
    if (min.x < b.min.x) i.min.x = b.min.x;
    if (min.y < b.min.y) i.min.y = b.min.y;
    if (min.z < b.min.z) i.min.z = b.min.z;

    if (max.x > b.max.x) i.max.x = b.max.x;
    if (max.y > b.max.y) i.max.y = b.max.y;
    if (max.z > b.max.z) i.max.z = b.max.z;
  } else {
    i = Box();
  }
  return i;
}

/** \brief Unions \p b with this one and returns the result.
 *
 * This returns the smallest Box that contains both this one and \p b.
 * Unless the boxes are neatly aligned, the result will also contain
 * lots of points outside the two, but you can't help that.
 *
 * This method commutes.
 *
 * \todo Use fewer branches.
 */
template<typename T>
Box<T> Box<T>::enclosure(const Box<T>& b) const {
  Box u(*this);
  if (min.x > b.min.x) u.min.x = b.min.x;
  if (min.y > b.min.y) u.min.y = b.min.y;
  if (min.z > b.min.z) u.min.z = b.min.z;

  if (max.x < b.max.x) u.max.x = b.max.x;
  if (max.y < b.max.y) u.max.y = b.max.y;
  if (max.z < b.max.z) u.max.z = b.max.z;

  return u;
}

#endif // LIBSAKUSEN_METHOD_DEFINITIONS
}
#endif
