#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "world.h"
#include "partialmap.h"
#include "completeunit.h"

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
    __gnu_cxx::hash_map<uint32, CompleteUnit*> units;
  public:
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }
};

extern PartialWorld* world;

}}

#endif // PARTIALWORLD_H

