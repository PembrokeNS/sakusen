#ifndef BEAM_H
#define BEAM_H

#include "ray.h"

namespace sakusen {
namespace server {

/** \file
 * This defines the Beam class. */

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
class Beam: private Ray {
  private:
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
    Beam();
    Beam(const Beam&);
  protected:
    Beam(Time creation, Point<sint32> s, Point<sint32> d) :
      Ray(s,d), startTime(creation), endTime(endTime)
    {}

  public:
    /** The destructor currently does nothing. */
    virtual ~Beam() {}
    /** \name mechanics
     * These functions deal with game mechanics. */
    //@{
    /** \brief cause the Ray to be intersected with objects
     *
     * This method is probably called every tick by the ::World. It takes no
     * arguments, finds objects that cross the ::Beam's path, and does the
     * appropriate actions on those objects. It handles the penetration of the
     * Beam (i.e. what things it can go through).
     */
    void resolveIntersections(void);
    //@}
};

}}

#endif

