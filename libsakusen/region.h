#ifndef REGION_H
#define REGION_H

#include "regiontype.h"
#include "point.h"
#include "rectangle.h"
#include "box.h"
#include "ray.h"
#include "oarchive.h"

namespace sakusen {

class IUnitStatus;

/** \brief Abstract class representing some sort of data describing a region
 *
 * \note This class (and subclasses) should be immutable.  That is, all
 * methods should be const, and there should be no way to alter the region
 * once constructed.  With this, there should be no need to copy (the same
 * instance can instead be shared).
 */
template<typename T>
class LIBSAKUSEN_API Region {
  public:
    typedef boost::shared_ptr<Region> Ptr;
    typedef boost::shared_ptr<const Region> ConstPtr;
  protected:
    Region() {}
    Region(const Region&) {}
  public:
    virtual ~Region() {}
  protected:
    virtual void storeData(OArchive&) const = 0;
  public:
    /** \name Containment functions
     *
     * These functions return true iff \p point is inside this Region.
     * 
     * We can't use Point<T> because then Point<sint32>s become corrupted when
     * T=sint16, and we can't use a templated Point type because the functions
     * are virtual.  We could probably get away with allowing just
     * Point<sint64> but then there'll be lots of conversion and temporaries
     * happening.  So, we provide one function for each necessary Point
     * instantiation.
     */
    //@{
    virtual bool contains(const Point<sint16>& point) const = 0;
    virtual bool contains(const Point<sint32>& point) const = 0;
    //@}

    /** \brief Is the unit inside the Region?
     * \return true iff \p unit is inside this Region.
     *
     * \todo ATM, it just checks whether the reference point of \p unit is in.
     * Possibly it might be necessary to properly intersect the unit with the
     * RegionData, otherwise it might be impossible to do things like damage
     * large units with area-of-effect weapons.
     */
    inline bool contains(const IUnitStatus* unit) const;

    /** \name Truncation methods
     *
     * These methods are for turning a Point into one inside the Region.
     *
     * If the Point is inside the region, a copy is returned. Otherwise, the
     * Point returned will be one inside the Region. No other properties of the
     * Point are guaranteed, though it will probably be vaguely in the same
     * direction from the origin as the original.
     *
     * This use is mostly intended for when using a Region to represent limits
     * on velocity or acceleration, to turn a requested velocity into a
     * possible one. It's probably sensible to have the client do cleverer
     * things to make this work, like scale the velocity rather than moving it
     * to a nearby vector, or plot a different route.
     *
     * Each of Point<sint16> and Point<sint32> is accepted rather than just
     * Point<T>, because a Region<sint16> can still be given a Point<sint32> to
     * truncate, and we don't want to suffer data loss before we even get into
     * the function.
     */
    //@{
    virtual Point<sint16> truncateToFit(const Point<sint16>&) const = 0;
    virtual Point<sint32> truncateToFit(const Point<sint32>&) const = 0;
    //@}

    /** \brief Get the centre of the Region.
     *
     * Will calculate and return a Point representing the 'best' part of the
     * Region, which is probably its centre (if the centre is inside the
     * Region). It is guaranteed that the returned Point is inside the Region,
     * unless the Region is empty.
     */
    virtual Point<T> getBestPosition() const = 0;

    /** \brief Get the smallest Rectangle that completely encloses the Region.
     *
     * It is guaranteed that every point inside this is also inside the
     * bounding rectangle, but not vice-versa. If the region is empty, the
     * bounding rectangle will also be empty.
     */
    virtual Rectangle<T> getBoundingRectangle() const = 0;

    /** \brief Get the smallest Box that completely encloses the Region.
     *
     * It is guaranteed that every point inside this is also inside the
     * bounding box, but not vice-versa. If the region is empty, the
     * bounding box will also be empty.
     */
    virtual Box<T> getBoundingBox() const = 0;

    /** \brief Intersect a Ray with the region.
     *
     * \param[in] r The Ray to trace.
     * \return The distance parameter for the ray when it hits the boundary of
     * the region, or +Inf if there is no intersection. If the ray starts inside
     * the region, you still get the first intersection with the boundary.
     */
    virtual double intersect(const Ray& r) const = 0;
    
    /** \brief Get the type of Region
     *
     * Used for (de)serialization.  Preferably shouldn't be needed otherwise.
     */
    virtual RegionType getType() const = 0;

    void store(OArchive&) const;
    static Ptr loadNew(IArchive&);
};

}

#endif // REGION_H

