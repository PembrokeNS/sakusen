#ifndef BEAM_H
#define BEAM_H

class Beam;

#include "ray.h"
#include "beamtype.h"

/** \file
 * This defines the ::Beam class. */

/** \brief a beam that physically exists, like a laser
 *
 * This class represents a beam that physically exists in the world i.e. it can
 * hit things and interact with them. This is distinct from a ::Ray that is just
 * used to represent a semi-infinite line and is a tool for collision-detection.
 * While things like line-of-sight determination will use a Ray to detect
 * objects (Units and Effects) in a given direction, the Beam is sent out into
 * the world, probably gets drawn on the user's display, and affects objects
 * when it hits them.
 */
class Beam: private Ray {
  private:
    BeamTypeID type;
    /** when the beam sprang into existence */
    Time startTime;
    /** The beam will stop at this time at the
     * latest: it may stop earlier e.g. if
     * the ::Unit that created it is destroyed.
     */
    Time endTime;
    
    /** The default ctr is private to stop people creating an uninitialized
     * Beam.
     */
    Beam();
  protected:
    Beam(BeamTypeID t, Time creation, Point<sint32> s, Point<sint32> d) :
      Ray(s,d), type(t), startTime(creation), endTime(creation+t->duration)
    {}

  public:
    /** The destructor currently does nothing. */
    ~Beam() {}
    /** \name mechanics
     * These functions deal with game mechanics. */
    //@{
    /** \brief factory method
     *
     * Creates a new ::Beam with the given parameters and does all the necessary
     * things to ensure it is registered as an active object in the world.
     */
    static void spawn(
        BeamTypeID type,
        Time creationTime,
        Point<sint32> position,
        Point<sint32> direction
      );
    /** \brief cause the ::Ray to be intersected with objects
     *
     * This method is probably called every tick by the ::World. It takes no
     * arguments, finds objects that cross the ::Beam's path, and does the
     * appropriate actions on those objects. It handles the penetration of the
     * ::Beam (i.e. what things it can go through).
     */
    void resolveIntersections(void);
    //@}
};

#endif

