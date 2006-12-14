#ifndef BALLISTIC_H
#define BALLISTIC_H

#include "libsakusen-global.h"
#include "quadratic.h"
#include "point.h"
#include "ref.h"
#include "playerid.h"

namespace sakusen {
namespace server {

/** \brief Represents a ballistic projectile, like a bullet.
 *
 * This is a lightweight object that represents an object whose path is a
 * Quadratic. It is intended mainly for bullets and the like, to save having
 * to construct a (quite heavy) unit for every bullet, which, if you have a
 * load of Peewees firing 10 rounds per second, might be a bit OTT.
 *
 * To reflect this lightweightness, Ballistics are supposed to be created once,
 * maybe drawn a few times, but otherwise left in peace until they collide with
 * something.  Effects probably do not want to modify Ballistics, Ballistics do
 * not affect each other (e.g. by colliding), and they do not get destroyed
 * until they collide with something.
 */
class Ballistic : public virtual IReferent {
  private:
    PlayerID owner;
    Quadratic path;
  private:
    /** Default constructor, private so you are forced to construct with
     * arguments.
     */
    Ballistic();
    Ballistic(const Ballistic&);
  protected:
    /** \brief Main constructor.
     *
     * All the parameters should be pretty obvious. */
    Ballistic(
        PlayerID owner,
        Time startTime,
        Point<sint32> startPosition,
        Point<sint32> startVelocity
      );
    /** \brief Construct from Quadratic
     *
     * This constructor does the same as the previous one but takes the
     * arguments wrapped up in an existing Quadratic, of which a private copy
     * is taken.
     */
    Ballistic(PlayerID owner, Quadratic p);

    /** \brief Collision callback
     *
     * Callback used when the Ballistic hits something */
    virtual void onCollision(const Point<sint32>& pos) = 0;
  public:
    /** The destructor currently does nothing. */
    virtual ~Ballistic() {}

    inline PlayerID getOwner() const { return owner; }
    inline const Quadratic& getPath() const { return path; }

    /** \name mechanics
     * Functions to deal with game mechanics */
    //@{
    /** \brief gets the position
     *
     * This trivial function causes the underlying
     * Quadratic to be evaluated for
     * time \p t, returning the position in space that the projectile occupies
     * at this time.
     */
    inline Point<sint32> getPosition(Time t) {return path.evaluate(t);}

    bool resolveIntersections();
    //@}
};

}}

#endif

