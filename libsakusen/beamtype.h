#ifndef BEAMTYPE_H
#define BEAMTYPE_H

#include "visibility.h"

namespace sakusen {

/** \file
 * This file contains the class ::BeamType */
/** \brief This class represents a type of ::Beam, for example a +1 fiery laser
 * of death.
 *
 * This trivial class is intended to be subclassed with different callbacks to
 * perform actions on hitting things, probably involving some kind of fiery
 * death.
 */
class BeamType {
  friend class Beam;
  private:
  /** the visibility of this beam
   * \todo As beams can be quite long, we need to think about what we will do if
   * someone can see one end of a beam but not the other, or just the middle, or
   * even the start and end of a bean but not the middle.
   */
    Visibility vis;
    /** The flags store information on what the beam interacts with and what it
     * can go through. See the enumeration below.
     */
    uint8 flags;
    /** This is true iff the beam can only go a finite distance.
     * \todo The distance is not set anywhere.
     */
    bool finite;
    /** This is the length of time the beam will last. If you do not know that
     * length of time in advance, set it to 0 and explicitly cause the beam to
     * be destroyed at the appropriate time.
     */
    Time duration; /* 0 iff it lasts 'til explicitly stopped */
  protected:
    //@{
    /** the constructors are protected so only subclasses can call them */
    BeamType();
    BeamType(const BeamType&);
    //@}
  public:
    /** This enumeration is used for the ::flags field. For each type of hit,
     * 'interactWith' is set iff the appropriate callback should be called on
     * this type of collision. 'passThrough' is clear iff the beam should stop
     * once it hits one of this type of thing.
     */
    enum {
      interactWithUnits = 1,
      passThroughUnits = 2,
      interactWithLand = 4,
      passThroughLand = 8,
      interactWithWater = 16,
      passThroughWater = 32,
    };
    ~BeamType();
    //@{
    /* \name accessors */
    /** These allow you to examine the ::flags. */
    inline bool getInteractWithUnits(void) {return ((flags & interactWithUnits) != 0);}
    inline bool getPassThroughUnits(void) {return ((flags & passThroughUnits) != 0);}
    inline bool getInteractWithLand(void) {return ((flags & interactWithLand) != 0);}
    inline bool getPassThroughLand(void) {return ((flags & passThroughLand) != 0);}
    inline bool getInteractWithWater(void) {return ((flags & interactWithWater) != 0);}
    inline bool getPassThroughWater(void) {return ((flags & passThroughWater) != 0);}
    //@}
};

typedef const BeamType* BeamTypeID;

}

#endif // BEAMTYPE_H

