#ifndef LIBSAKUSEN__WORLD_H
#define LIBSAKUSEN__WORLD_H

#include "libsakusen-global.h"
#include "ballistic.h"
#include "beam.h"
#include "universe.h"
#include "map.h"

using namespace sakusen;

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
    std::list<Ballistic> ballistics;
    std::list<Beam> beams;
    Time timeNow; /* Current game time */
  public:
    /* accessors */
    inline const Universe* getUniverse(void) const { return universe; }
    virtual Map* getMap(void) = 0;
    virtual const Map* getMap(void) const = 0;
    inline void addBallistic(const Ballistic& ballistic)
    {
      ballistics.push_back(ballistic);
    }
    inline void addBeam(const Beam& beam)
    {
      beams.push_back(beam);
    }
    inline const Time& getTimeNow(void) { return timeNow; }
};

extern LIBSAKUSEN_API World* world;

#endif // LIBSAKUSEN__WORLD_H

