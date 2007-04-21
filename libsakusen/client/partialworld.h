#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "hash_list.h"
#include "idindex.h"
#include "world.h"
#include "partialmap.h"
#include "updatedunit.h"
#include "updatedsensorreturns.h"
#include "update.h"
#include "rectangle.h"

namespace sakusen {
namespace client {

class LIBSAKUSEN_CLIENT_API PartialWorld : public World {
  public:
    PartialWorld(
        const Universe::ConstPtr& universe,
        PlayerID playerId,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        uint32 horizontalHeightfieldRes,
        uint32 verticalHeightfieldRes
      );
    ~PartialWorld();
    
    typedef IDIndex<uint32, UpdatedUnit, UpdatedUnitIDer> UnitIDIndex;
    typedef IDIndex<
        SensorReturnsID, UpdatedSensorReturns, UpdatedSensorReturnsIDer
      > SensorReturnsIDIndex;
  private:
    PlayerID playerId;
    PartialMap* map;
    hash_list<UpdatedUnit, Bounded, UpdatedUnit> units;
    UnitIDIndex::Ptr unitsById;
    
    hash_list<UpdatedSensorReturns, Bounded, UpdatedSensorReturns>
      sensorReturns;
    SensorReturnsIDIndex::Ptr sensorReturnsById;

    ISpatial::Ptr spatialIndex;

    __gnu_cxx::hash_map<uint32, Quadratic> ballistics;
    
    Ref<ISensorReturns> getISensorReturns(PlayerID player, SensorReturnsID id) {
      assert(player == playerId);
      return sensorReturnsById->find(id);
    }
    Ref<ICompleteUnit> getICompleteUnit(PlayerID player, uint32 uintId) {
      assert(player == playerId);
      return unitsById->find(uintId);
    }
  public:
    inline PlayerID getPlayerId() const { return playerId; }
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }
    inline UnitIDIndex::ConstPtr getUnitsById() const {
      return unitsById;
    }
    inline SensorReturnsIDIndex::ConstPtr getSensorReturnsById() const {
      return sensorReturnsById;
    }
    inline ISpatial::ConstPtr getSpatialIndex() const { return spatialIndex; }

    void applyUpdate(const Update&);
    void endTick();
};

extern LIBSAKUSEN_CLIENT_API PartialWorld* world;

}}

#endif // PARTIALWORLD_H

