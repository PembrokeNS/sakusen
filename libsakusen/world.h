#ifndef LIBSAKUSEN__WORLD_H
#define LIBSAKUSEN__WORLD_H

#include "libsakusen-global.h"
#include "universe.h"
#include "map.h"

namespace sakusen {

class LIBSAKUSEN_API World {
  private:
    World(const World& copy); /* Shouldn't use the copy constructor - mayhem
                                 will ensue because it will invalidate all
                                 pointers to game objects stored here */
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
};

extern LIBSAKUSEN_API World* world;

}

#endif // LIBSAKUSEN__WORLD_H

