#include "spheremap.h"

using namespace sakusen;

SphereMap::SphereMap(
    const Universe* universe,
    const String& internalName,
    const Point<uint32>& volume,
    const Heightfield& heightfield,
    uint16 gravity,
    const std::vector<MapPlayMode>& playModes
  ) :
  Map(
      universe, internalName, volume/uint32(2), volume/uint32(2) - volume,
      heightfield, gravity, playModes
    )
{
  if (height() != width()) {
    Fatal("spherical topology incompatible with non-square map");
  }
}

