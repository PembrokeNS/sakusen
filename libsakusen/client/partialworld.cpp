#include "partialworld.h"

#include "map-methods.h"
#include "naivespatial.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::client;

PartialWorld::PartialWorld(
    const Universe::ConstPtr& universe,
    PlayerId pI,
    Topology topology,
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity,
    uint32 horizontalHeightfieldRes,
    uint32 verticalHeightfieldRes
  ) :
  World(universe),
  playerId(pI),
  map(Map::newMap<PartialMap>(
        topology, topRight, bottomLeft, gravity,
        horizontalHeightfieldRes, verticalHeightfieldRes
      )),
  units()
{
  unitsById.reset(new UnitIdIndex());
  sensorReturnsById.reset(new SensorReturnsIdIndex());
  ballisticsById.reset(new BallisticIdIndex());
  spatialIndex.reset(new NaiveSpatial());
  units.registerIndex(IIndex<UpdatedUnit>::Ptr(unitsById));
  sensorReturns.registerIndex(
      IIndex<UpdatedSensorReturns>::Ptr(sensorReturnsById)
    );
  ballistics.registerIndex(IIndex<UpdatedBallistic>::Ptr(ballisticsById));
  units.registerIndex(IIndex<Bounded>::Ptr(spatialIndex));
  sensorReturns.registerIndex(IIndex<Bounded>::Ptr(spatialIndex));
  ballistics.registerIndex(IIndex<Bounded>::Ptr(spatialIndex));
  world = this;
}

PartialWorld::~PartialWorld()
{
  delete map;

  world = NULL;
}

/** \brief Apply incoming update.
 *
 * This method should be called by the client for each Update it receives from
 * the server (in the order received!).
 */
void PartialWorld::applyUpdate(const Update& update)
{
  switch (update.getType()) {
    case updateType_unitAdded:
      {
        UnitAddedUpdateData data = update.getUnitAddedData();
        Ref<UpdatedUnit> unit = unitsById->find(data.getUnit().getId());
        if (unit.isValid()) {
          Debug("adding unit of existing id");
          units.erase(unit);
        }
        units.push_back(new UpdatedUnit(data.getUnit()));
      }
      break;
    case updateType_unitRemoved:
      {
        UnitRemovedUpdateData data = update.getUnitRemovedData();
        Ref<UpdatedUnit> unit = unitsById->find(data.getId());
        if (!unit.isValid()) {
          Debug("tried to remove non-existant unit");
          break;
        }
        units.erase(unit);
      }
      break;
    case updateType_unitAltered:
      {
        UnitAlteredUpdateData data = update.getUnitAlteredData();
        Ref<UpdatedUnit> unit = unitsById->find(data.getUnit().getId());
        if (!unit.isValid()) {
          Debug("tried to alter non-existant unit");
          break;
        }
        unit->alter(data.getUnit());
      }
      break;
    case updateType_orderAccepted:
      {
        OrderAcceptedUpdateData data(update.getOrderAcceptedData());
        Ref<UpdatedUnit> unit = unitsById->find(data.getUnitId());
        if (!unit.isValid()) {
          Debug("Update for non-existant unit");
          break;
        }
        unit->orderAccepted(data);
      }
      break;
    case updateType_sensorReturnsAdded:
      {
        SensorReturnsAddedUpdateData data = update.getSensorReturnsAddedData();
        SensorReturnsId id = data.getSensorReturns().getId();
        Ref<UpdatedSensorReturns> sensorReturnsIt = sensorReturnsById->find(id);
        if (sensorReturnsIt.isValid()) {
          Debug("adding sensor returns of existing id");
          sensorReturns.erase(sensorReturnsIt);
        }
        sensorReturns.push_back(
            new UpdatedSensorReturns(data.getSensorReturns())
          );
      }
      break;
    case updateType_sensorReturnsRemoved:
      {
        SensorReturnsRemovedUpdateData data =
          update.getSensorReturnsRemovedData();
        Ref<UpdatedSensorReturns> returns =
          sensorReturnsById->find(data.getId());
        if (!returns.isValid()) {
          Debug("tried to remove non-existant SensorReturns");
          break;
        }
        sensorReturns.erase(returns);
      }
      break;
    case updateType_sensorReturnsAltered:
      {
        SensorReturnsAlteredUpdateData data =
          update.getSensorReturnsAlteredData();
        Ref<UpdatedSensorReturns> returns =
          sensorReturnsById->find(data.getSensorReturns().getId());
        if (!returns.isValid()) {
          Debug("tried to alter non-existant SensorReturns");
          break;
        }
        returns->alter(data.getSensorReturns());
      }
      break;
    case updateType_ballisticAdded:
      {
        BallisticAddedUpdateData data = update.getBallisticAddedData();
        ClientBallisticId id = data.getId();
        if (ballisticsById->count(id)) {
          Debug("adding Ballistic of existing id");
        }
        ballistics.push_back(new UpdatedBallistic(id, data.getPath()));
      }
      break;
    case updateType_ballisticRemoved:
      {
        BallisticRemovedUpdateData data = update.getBallisticRemovedData();
        Ref<UpdatedBallistic> ballistic = ballisticsById->find(data.getId());
        if (!ballistic.isValid()) {
          Debug("tried to remove non-existant Ballistic");
          break;
        }
        ballistics.erase(ballistic);
      }
      break;
    default:
      Fatal("unexpected UpdateType: " << update.getType());
  }
}

/** \brief Indicate end of tick.
 *
 * The client should call this method after applying all the Updates for a
 * single tick.  This is necessary because there are certain things that the
 * server assumes the client can figure out for itself, and these things must
 * be accounted for if there have been no Updates to the contrary.
 */
void PartialWorld::endTick()
{
  for (hash_list<UpdatedUnit, Bounded, UpdatedUnit>::iterator unit =
      units.begin(); unit != units.end(); ++unit) {
    (*unit)->incrementState();
  }
  ++timeNow;
}

PartialWorld* sakusen::client::world = NULL;

