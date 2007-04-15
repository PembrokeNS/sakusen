#ifndef LIBSAKUSEN_ISPATIAL_H
#define LIBSAKUSEN_ISPATIAL_H

#include <map>

#include "iindex.h"
#include "bounded.h"
#include "box.h"

namespace sakusen {

/** \brief An interface to a spatial index.
 *
 * This class provides a common interface to all types of spatial index. A
 * spatial index is a data structure that allows you to efficiently find
 * objects based on their position in space. A quadtree is an example of a
 * spatial index.
 *
 * In Sakusen, the spatial index will store references to only objects with
 * bboxes; that is, instances of Ref<Bounded>. Note that we use subtyping
 * rather than a parametric (templated) type because we will want to store
 * different types of objects in the same index, as long as they all have
 * bboxen.
 */
class ISpatial : public IIndex<Bounded> {
  public:
    typedef boost::shared_ptr<ISpatial> Ptr;
    typedef boost::shared_ptr<const ISpatial> ConstPtr;
  protected:
    ISpatial() {}
    ISpatial(const ISpatial&) : IIndex<Bounded>() {}
  public:
    virtual ~ISpatial() {}

    /** \brief Typedef for result type of all the methods which return some
     * objects */
    typedef std::vector<Ref<Bounded> > Result;

    /** \brief Find objects whose bboxes are entirely inside the given bbox. */
    /** \brief Find objects whose bboxes are partially or completely inside the given bbox. */

    /** \brief Find objects whose bbox bounds intersect the given bbox bounds
     * and which match the given filter. */
    virtual Result findIntersecting(
        const Box<sint32>&,
        const GameObject filter
      ) const = 0;

    /** \brief Find objects whose bbox bounds intersect the given rectangle
     * bounds and which match the given filter. */
    virtual Result findIntersecting(
        const Rectangle<sint32>& r,
        const GameObject filter
      ) const {
      return findIntersecting(Box<sint32>(r), filter);
    };

    /** \brief Find objects which might contain the given point, and
     * which match the given filter.  This method is permitted to overestimate
     * and return too many objects for the sake of speed. */
    virtual Result findFastContaining(
        const Position&,
        const GameObject filter
      ) const = 0;

    /** \brief Find objects which really contain the given point, and
     * which match the given filter. */
    virtual Result findContaining(
        const Position&,
        const GameObject filter
      ) const = 0;

    /** \brief Find objects whose bboxes partially or completely enclose the given bbox. */
    /** \brief Find objects whose bboxes completely enclose the given bbox. */
    
    /** \brief Find ray intersections with objects
     *
     * Returns a map, keyed by ray parameter, of all intersections of the given
     * ray with objects in the index subject to the object matching the given
     * \p filter, and the ray parameter of the intersection being at most
     * \p extent.  Note that this should be the intersection with the actual
     * object, not its bounding box.
     */
    virtual std::map<double, Ref<Bounded> > findIntersections(
        const Ray&,
        const double extent,
        const GameObject filter
      ) const = 0;
};

}

#endif /* LIBSAKUSEN_ISPATIAL_H */

