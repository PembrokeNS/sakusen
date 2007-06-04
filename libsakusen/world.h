#ifndef LIBSAKUSEN__WORLD_H
#define LIBSAKUSEN__WORLD_H

#include <boost/utility.hpp>

#include "libsakusen-global.h"
#include "universe.h"
#include "map.h"
#include "sensorreturnsid.h"
#include "ref.h"
#include "ispatial.h"

namespace sakusen {

class ISensorReturns;

/** \brief Manages everything about the game state during play.
 *
 * The World represents the entire game state during play.  There must be at
 * most one World in existence at any one time.  That one World will be pointed
 * to by sakusen::world (and other similar variables sakusen::server::world and
 * sakusen::client::world, although they have more specific types).
 *
 * World itself is fairly simple, but its subclasses in the client and server
 * libraries add the functionality needed in their respective contexts.
 */
class LIBSAKUSEN_API World : boost::noncopyable {
  protected:
    World(const Universe::ConstPtr& universe);
  public:
    virtual ~World();
  protected:
    /** \brief The universe used for this World */
    Universe::ConstPtr universe;
    /** \brief Current game time in ticks. */
    Time timeNow;
  public:
    /** \name Accessors. */
    //@{
    inline const Universe::ConstPtr& getUniverse() const { return universe; }
    virtual Map* getMap() = 0;
    virtual const Map* getMap() const = 0;
    inline const Time& getTimeNow() { return timeNow; }
    //@}

    /** \brief Get a particular SensorReturns
     *
     * Finds a SensorReturns based on its owner and id
     */
    virtual Ref<ISensorReturns> getISensorReturns(
        PlayerID player, SensorReturnsID id
      ) = 0;
    /** \brief Get a particular unit
     *
     * Finds a unit based on its owner and id
     */
    virtual Ref<ICompleteUnit> getICompleteUnit(
        PlayerID player, uint32 unitId
      ) = 0;
    /** \brief Get a spatial index for the objects in the World */
    virtual ISpatial::ConstPtr getSpatialIndex() const = 0;
};

/** \brief Pointer to the game world.
 *
 * This pointer is assigned to by the World constructor and reset to NULL by
 * its destructor.  Users of the library should never have to explicitly assign
 * to it.
 */
extern LIBSAKUSEN_API World* world;

}

#endif // LIBSAKUSEN__WORLD_H

