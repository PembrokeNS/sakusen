#ifndef BALLISTICTYPE_H
#define BALLISTICTYPE_H

#include "point.h"
#include "visibility.h"

namespace sakusen {

class ICompleteUnit;

/** \file
 * This file declares the ::BallisticType class. */

/** \brief the type of a ::Ballistic
 *
 * This class represents a type of ::Ballistic, e.g. a machine-gun bullet, or a
 * mortar shell. It contains all the data that are shared by the same kind of
 * projectile, like its Visibility, and what happens when it hits something.
 *
 * This class is intended to be overridden so you can define BallisticTypes that
 * do special actions, like damaging the thing they hit, or spawning a smoke
 * effect.
 */
class BallisticType {
  friend class Ballistic;
  private:
    /** represents the visibility of a ::Ballistic of this type */
    Visibility vis;

  protected:
    /** The default constructor does nothing at present but is protected so only
     * subclasses can call it.
     */
    BallisticType() {}
    /** The copy constructor does nothing at present but is protected so only
     * subclasses can call it.
     */
    BallisticType(const BallisticType&);
  public:
    /** The destructor does nothing but is virtual so subclasses can override it
     * if necessary.
     */
    virtual ~BallisticType();

    /** returns by reference the visibility of a ::Ballistic of this type */
    inline Visibility& getVisibility(void) {return vis;}
    /** \brief callback for hitting a ::Unit
     * 
     * Subclasses may override this method to perform some action on hitting a
     * Unit. The default action is a nop.
     */
    virtual void hitUnit(ICompleteUnit*) {}
    /** \brief callback for hitting the ground (probably representing a miss)
     * 
     * Subclasses may override this method to perform some action on hitting the
     * ground. The default action is a nop, which may be appropriate for some
     * projectiles.
     */
    virtual void hitLandscape(Point<sint32>&) {}
};

}

#endif // BALLISTICTYPE_H

