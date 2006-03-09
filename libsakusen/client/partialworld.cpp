#include "partialworld.h"

#include "map-methods.h"

using namespace std;
using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::client;

PartialWorld::PartialWorld(
    const Universe* universe,
    Topology topology,
    const Point<sint32>& topRight,
    const Point<sint32>& bottomLeft,
    uint16 gravity
  ) :
  World(universe),
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
  /* TODO: make this fast by storing data sensibly */
  list<UpdatedUnit*> result;

  for (hash_map<uint32, UpdatedUnit*>::iterator unit = units.begin();
      unit != units.end(); ++unit) {
    if (rect.fastIntersects(unit->second)) {
      result.push_back(unit->second);
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

