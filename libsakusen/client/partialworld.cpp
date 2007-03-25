#include "partialworld.h"

#include "map-methods.h"
#include "naivespatial.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::client;

PartialWorld::PartialWorld(
    const Universe::ConstPtr& universe,
    PlayerID pI,
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
  spatialIndex.reset(new NaiveSpatial());
  /** \bug This spatial index never gets anything added to it */
  world = this;
}

PartialWorld::~PartialWorld()
{
  while (!units.empty()) {
    hash_map<uint32, UpdatedUnit*>::iterator b = units.begin();
    delete b->second;
    units.erase(b);
  }

  sensorReturnsById.clear();

  delete map;

  world = NULL;
}

/** \brief Return a list of units intersecting the given rectangle
 *
 * \warning For speed, this may return a few extra units that do not in fact
 * intersect the rectangle */
list<UpdatedUnit*> PartialWorld::getUnitsIntersecting(
    const Rectangle<sint32>& rect
  )
{
  /*QDebug("checking " << units.size() << " units for intersection");*/
  /** \todo make this fast by storing data sensibly */
  list<UpdatedUnit*> result;

  for (hash_map<uint32, UpdatedUnit*>::iterator unit = units.begin();
      unit != units.end(); ++unit) {
    if (rect.fastIntersects(unit->second)) {
      result.push_back(unit->second);
    }
  }

  return result;
}

/** \brief Return a list of sensor returns intersecting the given rectangle
 *
 * \warning For speed, this may return a few extra sensor returns that do not
 * in fact intersect the rectangle */
list<Ref<UpdatedSensorReturns> > PartialWorld::getSensorReturnsIntersecting(
    const Rectangle<sint32>& rect
  )
{
  /** \todo make this fast by storing data sensibly */
  list<Ref<UpdatedSensorReturns> > result;

  for (hash_list<UpdatedSensorReturns>::iterator returns =
      sensorReturns.begin(); returns != sensorReturns.end(); ++returns) {
    if (rect.fastIntersects(*returns)) {
      result.push_back(*returns);
    }
  }

  return result;
}

void PartialWorld::applyUpdate(const Update& update)
{
  switch (update.getType()) {
    case updateType_unitAdded:
      {
        UnitAddedUpdateData data = update.getUnitAddedData();
        if (units.count(data.getUnit().getId())) {
          Debug("adding unit of existing id");
          delete units[data.getUnit().getId()];
        }
        units[data.getUnit().getId()] = new UpdatedUnit(data.getUnit());
      }
      break;
    case updateType_unitRemoved:
      {
        UnitRemovedUpdateData data = update.getUnitRemovedData();
        __gnu_cxx::hash_map<uint32, UpdatedUnit*>::iterator unit =
          units.find(data.getId());
        if (unit == units.end()) {
          Debug("tried to remove non-existant unit");
          break;
        }
        delete unit->second;
        units.erase(unit);
      }
      break;
    case updateType_unitAltered:
      {
        UnitAlteredUpdateData data = update.getUnitAlteredData();
        __gnu_cxx::hash_map<uint32, UpdatedUnit*>::iterator unit =
          units.find(data.getUnit().getId());
        if (unit == units.end()) {
          Debug("tried to alter non-existant unit");
          break;
        }
        unit->second->alter(data.getUnit());
      }
      break;
    case updateType_orderQueued:
      {
        OrderQueuedUpdateData data(update.getOrderQueuedData());
        hash_map<uint32, UpdatedUnit*>::iterator unit =
          units.find(data.getUnitId());
        if (unit == units.end()) {
          Debug("Update for non-existant unit");
          break;
        }
        unit->second->orderQueued(data);
      }
      break;
    case updateType_orderAccepted:
      {
        OrderAcceptedUpdateData data(update.getOrderAcceptedData());
        hash_map<uint32, UpdatedUnit*>::iterator unit =
          units.find(data.getUnitId());
        if (unit == units.end()) {
          Debug("Update for non-existant unit");
          break;
        }
        unit->second->orderAccepted(data);
      }
      break;
    case updateType_orderCompleted:
      {
        OrderCompletedUpdateData data(update.getOrderCompletedData());
        hash_map<uint32, UpdatedUnit*>::iterator unit =
          units.find(data.getUnitId());
        if (unit == units.end()) {
          Debug("Update for non-existant unit");
          break;
        }
        unit->second->orderCompleted(data);
      }
      break;
    case updateType_sensorReturnsAdded:
      {
        SensorReturnsAddedUpdateData data = update.getSensorReturnsAddedData();
        SensorReturnsID id = data.getSensorReturns().getId();
        if (sensorReturnsById.count(id)) {
          Debug("adding sensor returns of existing id");
          sensorReturns.erase(sensorReturnsById[id]);
          sensorReturnsById.erase(id);
        }
        sensorReturns.push_back(
            new UpdatedSensorReturns(data.getSensorReturns())
          );
        hash_list<UpdatedSensorReturns>::iterator newIt = sensorReturns.end();
        --newIt;
        sensorReturnsById[id] = newIt;
      }
      break;
    case updateType_sensorReturnsRemoved:
      {
        SensorReturnsRemovedUpdateData data =
          update.getSensorReturnsRemovedData();
        UpdatedSensorReturnsIt returns = sensorReturnsById.find(data.getId());
        if (returns == sensorReturnsById.end()) {
          Debug("tried to remove non-existant SensorReturns");
          break;
        }
        sensorReturns.erase(returns->second);
        sensorReturnsById.erase(returns);
      }
      break;
    case updateType_sensorReturnsAltered:
      {
        SensorReturnsAlteredUpdateData data =
          update.getSensorReturnsAlteredData();
        UpdatedSensorReturnsIt returns =
          sensorReturnsById.find(data.getSensorReturns().getId());
        if (returns == sensorReturnsById.end()) {
          Debug("tried to alter non-existant SensorReturns");
          break;
        }
        (*returns->second)->alter(data.getSensorReturns());
      }
      break;
    case updateType_ballisticAdded:
      {
        BallisticAddedUpdateData data = update.getBallisticAddedData();
        uint32 id = data.getId();
        if (ballistics.count(id)) {
          Debug("adding Ballistic of existing id");
        }
        ballistics[id] = data.getPath();
      }
      break;
    case updateType_ballisticRemoved:
      {
        BallisticRemovedUpdateData data =
          update.getBallisticRemovedData();
        __gnu_cxx::hash_map<uint32, Quadratic>::iterator ballistic =
          ballistics.find(data.getId());
        if (ballistic == ballistics.end()) {
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

void PartialWorld::endTick()
{
  for (hash_map<uint32, UpdatedUnit*>::iterator unit = units.begin();
      unit != units.end(); ++unit) {
    unit->second->incrementState();
  }
  ++timeNow;
}

PartialWorld* sakusen::client::world = NULL;

