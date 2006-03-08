#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "world.h"
#include "partialmap.h"
#include "updatedunit.h"
#include "update.h"
#include "rectangle.h"

namespace sakusen {
namespace client {

class LIBSAKUSEN_API PartialWorld : public World {
  private:
    PartialWorld();
    PartialWorld(const PartialWorld&);
  public:
    PartialWorld(
        const Universe* universe,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity
      );
    ~PartialWorld();
  private:
    PartialMap* map;
    __gnu_cxx::hash_map<uint32, UpdatedUnit*> units; /* units owned by this */
  public:
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }

    std::list<UpdatedUnit*> getUnitsIntersecting(const Rectangle<sint32>&);

    void applyUpdate(const Update&);
    void endTick();
};

extern PartialWorld* world;

}}

#endif // PARTIALWORLD_H

