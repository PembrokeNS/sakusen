#ifndef LIBSAKUSEN_SERVER__EFFECT_H
#define LIBSAKUSEN_SERVER__EFFECT_H

namespace sakusen {
namespace server {

class Effect;

}}

#include <sakusen/server/global.h>
#include <sakusen/point.h>
#include <sakusen/region.h>
#include <sakusen/playerid.h>
#include <sakusen/visibility.h>
#include <sakusen/server/layeredunit.h>

namespace sakusen {
namespace server {

/** \brief Represents an alteration to some region of space in the game.
 *
 * \todo Some kind of visibility modification scheme. */
class Effect : public Bounded {
  private:
    PlayerId owner;
    Region<sint32>::Ptr region;
    Visibility visibility;

    Effect();
  protected:
    Effect(const Effect& copy) :
      Bounded(copy),
      owner(copy.owner),
      region(copy.region),
      visibility(copy.visibility)
    {}
    Effect(PlayerId o, const Region<sint32>::Ptr& r, Visibility v) :
      owner(o), region(r), visibility(v) {}

  public:
    virtual ~Effect() {}
    /** \name Accessors. */
    //@{
    inline GameObject getObjectType() const { return gameObject_effect; }
    inline const PlayerId getOwner(void) const { return owner; }
    inline const Region<sint32>::Ptr& getRegion(void) const { return region; }
    inline const Visibility& getVisibility(void) const { return visibility; }
    //@}
    /* callbacks */
    /** \brief Callback called each tick with every unit within the Effect.
     *
     * Can be overriden by subclasses.  Otherwise, the default implementation
     * does nothing. */
    virtual void onUnitPresent(const Ref<LayeredUnit>&) {}
    /** \brief Callback called when a unit enters the Effect.
     *
     * Can be overriden by subclasses.  Otherwise, the default implementation
     * does nothing. */
    virtual void onUnitEnter(const Ref<LayeredUnit>&) {}
    /** \brief Callback called when a unit leaves the Effect.
     *
     * Can be overriden by subclasses.  Otherwise, the default implementation
     * does nothing. */
    virtual void onUnitLeave(const Ref<LayeredUnit>&) {}
    /** \brief Test the effect for removal
     *
     * \return true iff the Effect should be removed from the World
     *
     * Called every tick by CompleteWorld to allow the Effect to remove itself,
     * after the onUnitPresent calls.
     *
     * The default implementation returns true, so, unless overridden, the
     * Effect will last for but a single tick. */
    virtual bool onRemovalTest(void) { return true; }

    Box<sint32> getBoundingBox() const { return region->getBoundingBox(); }
    /* Overloading fastIntersect and fastContains because for all the regions
     * we presently have, it's not worth primary-testing intersection or
     * containment against the bounding box
     * for the region.  If we get more complicated sorts of region, then may
     * have to be rethought */
    boost::tuple<double,double> fastIntersect(const Ray& r) const {
      return region->intersect(r);
    }
    boost::tuple<double,double> intersect(const Ray& r) const {
      return region->intersect(r);
    }
    bool fastContains(const Position& p) const { return region->contains(p); }
    bool contains(const Position& p) const { return region->contains(p); }
};

}}

#endif // LIBSAKUSEN_SERVER__EFFECT_H

