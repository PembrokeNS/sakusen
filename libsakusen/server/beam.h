#ifndef BEAM_H
#define BEAM_H

#include "ray.h"
#include "ref.h"
#include "layeredunit.h"
#include "effect.h"

namespace sakusen {
namespace server {

/** \brief a beam that physically exists, like a laser
 *
 * This class represents a beam that physically exists in the world i.e. it can
 * hit things and interact with them. This is distinct from a Ray that is just
 * used to represent a semi-infinite line and is a tool for collision-detection.
 * While things like line-of-sight determination will use a Ray to detect
 * objects (Units and Effects) in a given direction, the Beam is sent out into
 * the world, probably gets drawn on the user's display, and affects objects
 * when it hits them.
 */
class Beam : private Ray, public virtual IReferent {
  private:
    /** \brief The Unit producing the Beam */
    Ref<LayeredUnit> source;
    /** when the beam sprang into existence */
    Time startTime;
    /** The beam will stop at this time at the
     * latest: it may stop earlier e.g. if
     * the unit that created it is destroyed.
     */
    Time endTime;
    
    /** The default ctr is private to stop people creating an uninitialized
     * Beam.
     */
    Beam(const Beam&);
  protected:
    Beam(
        const Point<sint32>& start,
        const Point<sint16>& direction,
        const Ref<LayeredUnit>& s,
        Time creation,
        TimeSpan duration
      );

  public:
    virtual ~Beam();
    virtual void onInteract(const Intersection&);
    virtual void onInteractLand(double /*position*/) {}
    virtual void onInteractWater(double /*position*/) {}
    virtual void onInteractUnit(double /*position*/, const Ref<LayeredUnit>&) {}
    virtual void onInteractEffect(double /*position*/, const Ref<Effect>&) {}
    virtual bool onRemovalTest();
    /** \name mechanics
     * These functions deal with game mechanics. */
    //@{
    void resolveIntersections(void);
    //@}
};

}}

#endif

