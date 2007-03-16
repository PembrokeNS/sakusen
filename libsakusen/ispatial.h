#ifndef LIBSAKUSEN_ISPATIAL_H
#define LIBSAKUSEN_ISPATIAL_H

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
      ) = 0;

    /** \brief Find objects whose bboxes partially or completely enclose the given bbox. */
    /** \brief Find objects whose bboxes completely enclose the given bbox. */
    /* find ray intersections with bboxes */
};

}

#endif /* LIBSAKUSEN_ISPATIAL_H */

