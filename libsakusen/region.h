#ifndef REGION_H
#define REGION_H

#include "regiontype.h"
#include "point.h"
#include "rectangle.h"
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
    /** \brief Is the Point inside the Region?
     * \return true iff \p point is inside this Region.
     */
    virtual bool contains(const Point<T>& point) const = 0;

    /** \brief Is the unit inside the Region?
     * \return true iff \p unit is inside this Region.
     *
     * \todo ATM, it just checks whether the reference point of \p unit is in.
     * Possibly it might be necessary to properly intersect the unit with the
     * RegionData, otherwise it might be impossible to do things like damage
     * large units with area-of-effect weapons.
     */
    inline bool contains(const IUnitStatus* unit) const;

    /** \brief Turn a Point into one inside the Region.
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
     */
    virtual Point<T> truncateToFit(const Point<T>&) const = 0;
    
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
    
    virtual RegionType getType() const = 0;

    void store(OArchive&) const;
    static Ptr loadNew(IArchive&);
};

}

#endif // REGION_H

