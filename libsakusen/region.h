#ifndef REGION_H
#define REGION_H

#include "point.h"
#include "oarchive.h"
#include "iarchive.h"
#include "regiondata.h"
#include "rectangle.h"

namespace sakusen {

/** \brief Wrapper for abstract RegionData to save memory management for Region
 * users
 *
 * \note This class should be immutable.  That is, all methods should be
 * const, and there should be no way to alter the region once constructed. */
template<typename T>
class LIBSAKUSEN_API Region {
  public:
    typedef boost::shared_ptr<Region> Ptr;
    typedef boost::shared_ptr<const Region> ConstPtr;
    /** \brief Constructs region with given data.
     *
     * \param d data to use.  Ownership of the pointer is transferred to the
     * newly constructed Region.  Must not be NULL */
    Region(RegionData<T>* d) : data(d) { assert(data); }
  private:
    typename RegionData<T>::ConstPtr data; /* owned by this */
  public:
    /** \brief Is the Point inside the Region?
     * \return true iff \p point is inside this Region.
     */
    inline bool contains(const Point<T>& point) const;

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
    inline Point<T> truncateToFit(const Point<T>&) const;

    /** \brief Get the centre of the Region.
     *
     * Will calculate and return a Point representing the 'best' part of the
     * Region, which is probably its centre (if the centre is inside the
     * Region). It is guaranteed that the returned Point is inside the Region,
     * unless the Region is empty.
     */
    inline Point<T> getBestPosition() const;

    /** \brief Get the smallest Rectangle that completely encloses the Region.
     *
     * It is guaranteed that every point inside this is also inside the
     * bounding rectangle, but not vice-versa. If the region is empty, the
     * bounding rectangle will also be empty.
     */
    inline Rectangle<T> getBoundingRectangle() const;
    
    void store(OArchive&) const;
    static Region<T> load(IArchive&);
};

}
#endif // REGION_H

