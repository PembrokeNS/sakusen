#include "box.h"

/** \file
 * This file defines the ::Box class.
 */
namespace sakusen {

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
 * This returns the smallest ::Box that contains both this one and \p b.
 * Unless the boxes are neatly aligned, the result will also contain
 * lots of points outside the two, but you can't help that.
 *
 * If both boxes are empty, the result will not necessarily be empty. It
 * will be the minimal ::Box that contains the corners of both. If that
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
