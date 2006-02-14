#include "partialworld.h"

#include "map-methods.h"

using namespace __gnu_cxx;

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

PartialWorld* sakusen::client::world = NULL;

