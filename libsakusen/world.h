#ifndef LIBSAKUSEN__WORLD_H
#define LIBSAKUSEN__WORLD_H

#include <boost/utility.hpp>

#include "libsakusen-global.h"
#include "universe.h"
#include "map.h"
#include "sensorreturnsid.h"
#include "ref.h"

namespace sakusen {

class ISensorReturns;

class LIBSAKUSEN_API World : boost::noncopyable {
  protected:
    World(const Universe* universe);
  public:
    virtual ~World();
  protected:
    const Universe* universe; /* not owned by this */
    Time timeNow; /* Current game time */
  public:
    /* accessors */
    inline const Universe* getUniverse(void) const { return universe; }
    virtual Map* getMap(void) = 0;
    virtual const Map* getMap(void) const = 0;
    inline const Time& getTimeNow(void) { return timeNow; }
    virtual Ref<ISensorReturns> getISensorReturns(
        PlayerID player, SensorReturnsID id
      ) = 0;
};

extern LIBSAKUSEN_API World* world;

}

#endif // LIBSAKUSEN__WORLD_H

