#include "partialworld.h"

#include "map-methods.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::client;

PartialWorld::PartialWorld(
    const Universe* universe,
    PlayerID pI,
    Topology topology,
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity
  ) :
  World(universe),
  playerId(pI),
  map(Map::newMap<PartialMap>(topology, topRight, bottomLeft, gravity)),
  units()
{
  world = this;
}

PartialWorld::~PartialWorld()
{
  while (!units.empty()) {
    hash_map<uint32, UpdatedUnit*>::iterator b = units.begin();
    delete b->second;
    units.erase(b);
  }

  while (!sensorReturns.empty()) {
    hash_map<SensorReturnsID, UpdatedSensorReturns*>::iterator it =
      sensorReturns.begin();
    invalidateSensorReturnsRefs(it->first);
    delete it->second;
    sensorReturns.erase(it);
  }

  delete map;

  world = NULL;
}

void PartialWorld::invalidateSensorReturnsRefs(SensorReturnsID id)
{
  pair<
      __gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::iterator,
      __gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::iterator
    > refRange = sensorReturnRefs.equal_range(id);
  for (__gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::
      iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    refIt->second->invalidate();
  }
  sensorReturnRefs.erase(refRange.first, refRange.second);
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
list<UpdatedSensorReturns*> PartialWorld::getSensorReturnsIntersecting(
    const Rectangle<sint32>& rect
  )
{
  /** \todo make this fast by storing data sensibly */
  list<UpdatedSensorReturns*> result;

  for (hash_map<SensorReturnsID, UpdatedSensorReturns*>::iterator returns =
      sensorReturns.begin(); returns != sensorReturns.end(); ++returns) {
    if (rect.fastIntersects(returns->second)) {
      result.push_back(returns->second);
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
        if (sensorReturns.count(id)) {
          Debug("adding sensor returns of existing id");
          invalidateSensorReturnsRefs(id);
          delete sensorReturns[id];
        }
        sensorReturns[id] =
          new UpdatedSensorReturns(data.getSensorReturns());
      }
      break;
    case updateType_sensorReturnsRemoved:
      {
        SensorReturnsRemovedUpdateData data =
          update.getSensorReturnsRemovedData();
        __gnu_cxx::hash_map<SensorReturnsID, UpdatedSensorReturns*>::iterator
          returns = sensorReturns.find(data.getId());
        if (returns == sensorReturns.end()) {
          Debug("tried to remove non-existant SensorReturns");
          break;
        }
        invalidateSensorReturnsRefs(returns->first);
        delete returns->second;
        sensorReturns.erase(returns);
      }
      break;
    case updateType_sensorReturnsAltered:
      {
        SensorReturnsAlteredUpdateData data =
          update.getSensorReturnsAlteredData();
        __gnu_cxx::hash_map<uint32, UpdatedSensorReturns*>::iterator returns =
          sensorReturns.find(data.getSensorReturns().getId());
        if (returns == sensorReturns.end()) {
          Debug("tried to alter non-existant SensorReturns");
          break;
        }
        returns->second->alter(data.getSensorReturns());
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

void PartialWorld::registerRef(Ref<ISensorReturns>* ref)
{
  pair<SensorReturnsID, Ref<ISensorReturns>*> item((*ref)->getId(), ref);
  sensorReturnRefs.insert(item);
}

void PartialWorld::unregisterRef(Ref<ISensorReturns>* ref)
{
  pair<
      __gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::iterator,
      __gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::iterator
    > refRange = sensorReturnRefs.equal_range((*ref)->getId());
  
  /* This becomes slow if there are many Refs to the same SensorReturns object.
   * If that is a problem we could ID the refs too and have O(1) lookups for
   * them, but it's unlikely to be of help overall */
  for (__gnu_cxx::hash_multimap<SensorReturnsID, Ref<ISensorReturns>*>::
      iterator refIt = refRange.first; refIt != refRange.second;
      ++refIt) {
    if (refIt->second == ref) {
      sensorReturnRefs.erase(refIt);
      return;
    }
  }
  Fatal("tried to unregister a not-registered Ref");
}

PartialWorld* sakusen::client::world = NULL;

