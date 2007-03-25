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

class LIBSAKUSEN_API World : boost::noncopyable {
  protected:
    World(const Universe::ConstPtr& universe);
  public:
    virtual ~World();
  protected:
    Universe::ConstPtr universe;
    Time timeNow; /* Current game time */
  public:
    /* accessors */
    inline const Universe::ConstPtr& getUniverse() const { return universe; }
    virtual Map* getMap() = 0;
    virtual const Map* getMap() const = 0;
    inline const Time& getTimeNow() { return timeNow; }
    virtual Ref<ISensorReturns> getISensorReturns(
        PlayerID player, SensorReturnsID id
      ) = 0;
    virtual ISpatial::ConstPtr getSpatialIndex() const = 0;
};

extern LIBSAKUSEN_API World* world;

}

#endif // LIBSAKUSEN__WORLD_H

