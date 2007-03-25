#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "hash_list.h"
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
  private:
    PlayerID playerId;
    PartialMap* map;
    __gnu_cxx::hash_map<uint32, UpdatedUnit*> units; /* units owned by this */

    ISpatial::Ptr spatialIndex;
    
    hash_list<UpdatedSensorReturns> sensorReturns;
    __gnu_cxx::hash_map<
        SensorReturnsID,
        hash_list<UpdatedSensorReturns>::iterator
      > sensorReturnsById;
    typedef __gnu_cxx::hash_map<
        SensorReturnsID,
        hash_list<UpdatedSensorReturns>::iterator
      >::iterator UpdatedSensorReturnsIt;

    __gnu_cxx::hash_map<uint32, Quadratic> ballistics;
  public:
    inline PlayerID getPlayerId() const { return playerId; }
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }
    inline ISpatial::ConstPtr getSpatialIndex() const { return spatialIndex; }

    std::list<UpdatedUnit*> getUnitsIntersecting(const Rectangle<sint32>&);
    std::list<Ref<UpdatedSensorReturns> > getSensorReturnsIntersecting(
        const Rectangle<sint32>&
      );
    Ref<ISensorReturns> getISensorReturns(PlayerID player, SensorReturnsID id) {
      assert(player == playerId);
      UpdatedSensorReturnsIt it = sensorReturnsById.find(id);
      if (it == sensorReturnsById.end()) {
        return Ref<ISensorReturns>();
      }
      return *it->second;
    }

    void applyUpdate(const Update&);
    void endTick();
};

extern LIBSAKUSEN_CLIENT_API PartialWorld* world;

}}

#endif // PARTIALWORLD_H

