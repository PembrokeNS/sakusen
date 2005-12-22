#ifndef BALLISTIC_H
#define BALLISTIC_H

#include "libsakusen-global.h"
#include "ballistictypeid.h"
#include "ballistictype.h"
#include "quadratic.h"
#include "point.h"

/** \file
 * This file contains the ::Ballistic class. */

/** \brief Represents a ballistic projectile, like a bullet.
 *
 * This is a lightweight object that represents an object whose path is a
 * ::Quadratic. It is intended mainly for bullets and the like, to save having
 * to construct a (quite heavy) ::Unit for every bullet, which, if you have a
 * load of Peewees firing 10 rounds per second, might be a bit OTT.
 *
 * To reflect this lightweightness, Ballistics are supposed to be created once,
 * maybe drawn a few times, but otherwise left in peace until they collide with
 * something.  Effects probably do not want to modify Ballistics, Ballistics do
 * not affect each other (e.g. by colliding), and they do not get destroyed
 * until they collide with something.
 */
class Ballistic {
  private:
  BallisticTypeID type;
  Quadratic path;
  private:
  /** Default constructor, private so you are forced to construct with
   * arguments.
   */
  Ballistic();
  /** Main constructor. All the parameters should be pretty obvious. */
  Ballistic(
      BallisticTypeID type,
      Time startTime,
      Point<sint32> startPosition,
      Point<sint32> startVelocity
    );
  /** This constructor does the same as the previous one but takes the arguments
   * wrapped up in an existing ::Quadratic, of which a private copy is taken.
   */
  Ballistic(BallisticTypeID t, Quadratic p);
  public:
  /** The destructor currently does nothing. */
  ~Ballistic() {}

  /** \name mechanics
   * Functions to deal with game mechanics */
  //@{
  /* \brief Factory method, spawning a new ::Ballistic
   *
   * This factory method spawns a ::Ballistic with the given parameters and
   * ensures it is registered as an active ::Ballistic.
   */
  static void spawn(
      const BallisticTypeID type,
      const Time startTime,
      const Point<sint32>& startPosition,
      const Point<sint16>& startVelocity
    );
  /* \brief gets the position
   *
   * This trivial function causes the underlying ::Quadratic to be evaluated for
   * time \p t, returning the position in space that the projectile occupies at
   * this time.
   */
  inline Point<sint32> getPosition(Time t) {return path.evaluate(t);}
  //@}
};

#endif

