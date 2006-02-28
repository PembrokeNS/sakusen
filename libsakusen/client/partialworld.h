#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "world.h"
#include "partialmap.h"
#include "completeunit.h"
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
    __gnu_cxx::hash_map<uint32, CompleteUnit*> units; /* units owned by this */
  public:
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }

    std::list<CompleteUnit*> getUnitsIntersecting(const Rectangle<sint32>&);

    void applyUpdate(const Update&);
};

extern PartialWorld* world;

}}

#endif // PARTIALWORLD_H

