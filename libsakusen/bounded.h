#ifndef LIBSAKUSEN__BOUNDED_H
#define LIBSAKUSEN__BOUNDED_H

#include "libsakusen-global.h"
#include "point.h"
#include "ray.h"
#include "box.h"

namespace sakusen {

/** \brief A bounded region of or object in game-space.
 *
 * Any class that has meaningful extents in space should inherit from this
 * abstract class. Usually, subclasses represent tangible objects in the game
 * world, but they could also be abstract regions of game space; for example, a
 * unit's weapon range that the client wants to monitor for the presence of
 * enemy units.
 *
 * It demands methods for finding the bbox of the bounded object and spatially
 * relating with other game objects.
 */
class Bounded {
  public:
    /** \brief Virtual destructor.
     *
     * This destructor does nothing at present but needs to be virtual in case
     * subclasses want to override it.
     */
    virtual ~Bounded() {}
    /** \brief Retrieve the object's bbox.
     *
     * Really, you shouldn't call this method very often. If you want to do
     * operations on the bbox, you should edit this class to add operations
     * that delegate to the bbox.
     *
     * This method is pure virtual: subclasses should override it with the
     * appropriate code.
     */
    virtual Box<sint32> getBoundingBox(void) const = 0;

    /** \brief Intersect a Ray with the bbox.
     *
     * By default, this function calls getBoundingBox() and intersects the Ray
     * with the result, but it is left virtual so that subclasses can override
     * it (in case the actual intersection test is at least as fast as the bbox
     * intersection test, as for spheres).
     *
     * \param[in] r The Ray to trace.
     * \return The distance parameter for the ray when it hits the boundary of
     * the bbox, or +Inf if there is no intersection. If the ray starts inside
     * the bbox, you still get the first intersection with the boundary.
     */
    virtual double fast_intersect(const Ray& r) const;

    /** \brief Intersect a Ray with the actual object.
     *
     * This pure virtual method should be overridden by subclasses with the
     * appropriate code to return the first intersection of the ray with the
     * actual object bounded by the box.
     *
     * \param[in] r The Ray to trace.
     * \return The distance parameter for the ray when it hits the boundary of
     * the object, or +Inf if there is no intersection. If the ray starts
     * inside the object, you still get the first intersection with the
     * boundary.
     */
    virtual double intersect(const Ray& r) const = 0;

    /** \brief Intersect this bbox with another.
     *
     * This method decides whether the intersection of the regions enclosed by
     * the two bboxes is non-zero. That is, it will say yes iff the boundaries
     * overlap or one is completely enclosed by the other. It cannot be
     * overridden because the subclass would need to know the details of both
     * objects to do it sensibly.
     *
     * It is named "fast" because it operates only on the bboxes rather than
     * the bounded objects, even though there is no corresponding "slow"
     * operation in this case.
     *
     * \param[in] b The bounded object whose bbox should be intersected with
     * this one.
     * \return true iff the two bboxes intersect.
     */
    bool fast_intersection(const Bounded& b) const;
};

}

#endif
