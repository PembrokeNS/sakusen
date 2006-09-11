#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "world.h"
#include "partialmap.h"
#include "updatedunit.h"
#include "updatedsensorreturns.h"
#include "update.h"
#include "rectangle.h"

namespace sakusen {
namespace client {

class LIBSAKUSEN_CLIENT_API PartialWorld : public World {
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
    __gnu_cxx::hash_map<SensorReturnsID, UpdatedSensorReturns*> sensorReturns;
      /* sensor returns owned by this */
    __gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>
      sensorReturnRefs; /* These pointers not owned by this */

    void invalidateRefs(SensorReturnsID id);
  public:
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }

    std::list<UpdatedUnit*> getUnitsIntersecting(const Rectangle<sint32>&);
    std::list<UpdatedSensorReturns*> getSensorReturnsIntersecting(
        const Rectangle<sint32>&
      );
    ISensorReturns* getISensorReturns(PlayerID /*player*/, SensorReturnsID id) {
      /** \todo assert(correct player) */
      __gnu_cxx::hash_map<SensorReturnsID, UpdatedSensorReturns*>::iterator it =
        sensorReturns.find(id);
      if (it == sensorReturns.end()) {
        return NULL;
      }
      return it->second;
    }

    void applyUpdate(const Update&);
    void endTick();
    
    void registerRef(Ref<ISensorReturns>* ref);
    void unregisterRef(Ref<ISensorReturns>* ref);
};

extern LIBSAKUSEN_CLIENT_API PartialWorld* world;

}}

#endif // PARTIALWORLD_H

