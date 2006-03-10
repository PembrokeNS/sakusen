#ifndef BOX_H
#define BOX_H

#include "libsakusen-global.h"
#include "point.h"

namespace sakusen {

/** Represents an axis-aligned box (cube) in 3D space.
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
    /** Uses the default ::Point twice to create an empty ::Box. */
    Box(): min(), max() {}
    /** Creates a ::Box with the given corners. The caller is
     * responsible for ensuring that \p x1 \<= \p x2.
     */
    Box(Point<T> x1, Point<T> x2): min(x1), min(x2) {}
    Box(const Box<T>& b): min(b.min), max(b.max) {}
    ~Box() {}
    /** For a ::Box \c b, \c b[0] is the lower-left corner, and \c b[1]
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

    inline bool operator==(const Box b) const {
      return min == b.min && max == b.max;
    }

    inline std::ostream& operator<<(std::ostream& out) {
      out << "box <" << min << "," << max << ">";
      return out;
    }

    /** Tests whether \c this is a valid ::Box. */
    inline bool isValid() const {
      return (min < max);
    }

    /** \brief Tests whether this ::Box is empty (i.e. there does not exist
     * a ::Point inside it).
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
};

}
#endif
