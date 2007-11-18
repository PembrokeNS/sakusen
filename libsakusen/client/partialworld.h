#ifndef PARTIALWORLD_H
#define PARTIALWORLD_H

#include "gnu_extensions.h"
#include "hash_list.h"
#include "idindex.h"
#include "world.h"
#include "partialmap.h"
#include "updatedunit.h"
#include "updatedsensorreturns.h"
#include "updatedballistic.h"
#include "update.h"
#include "rectangle.h"

namespace sakusen {
namespace client {

/** \brief Helps clients keep track of the game state.
 *
 * This class is intended to perform those actions common to all clients
 * required to accurately keep track of the client's perspective on the game.
 * The client should call applyUpdate with each Update it recieves from the
 * server, and then endTick each time it determines that a tick has ended.
 * PartialWorld will automatically interpret this in the correct manner and
 * provides means for the client to examine this information (e.g. a spatial
 * index of the player's units and SensorReturns).
 */
class LIBSAKUSEN_CLIENT_API PartialWorld : public World {
  public:
    PartialWorld(
        const Universe::ConstPtr& universe,
        PlayerId playerId,
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        uint32 xyHeightfieldRes,
        uint32 zHeightfieldRes
      );
    ~PartialWorld();
    
    typedef IdIndex<UnitId, UpdatedUnit, UpdatedUnitIder> UnitIdIndex;
    typedef IdIndex<
        SensorReturnsId, UpdatedSensorReturns, UpdatedSensorReturnsIder
      > SensorReturnsIdIndex;
    typedef IdIndex<ClientBallisticId, UpdatedBallistic, UpdatedBallisticIder>
      BallisticIdIndex;
  private:
    PlayerId playerId;
    PartialMap* map;
    hash_list<UpdatedUnit, Bounded, UpdatedUnit> units;
    UnitIdIndex::Ptr unitsById;
    
    hash_list<UpdatedSensorReturns, Bounded, UpdatedSensorReturns>
      sensorReturns;
    SensorReturnsIdIndex::Ptr sensorReturnsById;

    hash_list<UpdatedBallistic, Bounded, UpdatedBallistic> ballistics;
    BallisticIdIndex::Ptr ballisticsById;

    ISpatial::Ptr spatialIndex;
    
    Ref<ISensorReturns> getISensorReturns(PlayerId player, SensorReturnsId id) {
      assert(player == playerId);
      return sensorReturnsById->find(id);
    }
    Ref<ICompleteUnit> getICompleteUnit(PlayerId player, UnitId uintId) {
      assert(player == playerId);
      return unitsById->find(uintId);
    }
  public:
    /** \name Accessors. */
    //@{
    inline PlayerId getPlayerId() const { return playerId; }
    inline Map* getMap() { return map; }
    inline const Map* getMap() const { return map; }
    inline UnitIdIndex::ConstPtr getUnitsById() const {
      return unitsById;
    }
    inline SensorReturnsIdIndex::ConstPtr getSensorReturnsById() const {
      return sensorReturnsById;
    }
    inline ISpatial::ConstPtr getSpatialIndex() const { return spatialIndex; }
    //@}

    void applyUpdate(const Update&);
    void endTick();
};

extern LIBSAKUSEN_CLIENT_API PartialWorld* world;

}}

#endif // PARTIALWORLD_H

