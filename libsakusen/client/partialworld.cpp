#include "partialworld.h"

#include "map-methods.h"

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
    hash_map<uint32, CompleteUnit*>::iterator b = units.begin();
    delete b->second;
    units.erase(b);
  }

  delete map;

  world = NULL;
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
        units[data.getUnit().getId()] = new CompleteUnit(data.getUnit());
      }
      break;
    case updateType_unitRemoved:
      {
        UnitRemovedUpdateData data = update.getUnitRemovedData();
        __gnu_cxx::hash_map<uint32, CompleteUnit*>::iterator unit =
          units.find(data.getId());
        if (unit == units.end()) {
          Debug("tried to remove non-existant unit");
          break;
        }
        delete unit->second;
        units.erase(unit);
      }
      break;
    /* TODO: order updates */
    case updateType_orderQueued:
    case updateType_orderAccepted:
    case updateType_orderCompleted:
    default:
      Fatal("unexpected UpdateType: " << update.getType());
  }
}

PartialWorld* sakusen::client::world = NULL;

