#include "world.h"

#include <list>
#include <vector>

using namespace sakusen;

World::World(const Universe* u) :
  universe(u),
  timeNow(0)
{
  /* sanity check */
  if (world) {
    Debug("World constructed when world != NULL");
  }
  world = this;
}

World::~World()
{
  world = NULL;
}

LIBSAKUSEN_API World* sakusen::world = NULL;

