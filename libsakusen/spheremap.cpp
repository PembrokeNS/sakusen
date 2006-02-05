#include "spheremap.h"

using namespace sakusen;

SphereMap::SphereMap(const MapTemplate& t) :
  Map(t)
{
  if (height() != width()) {
    Fatal("spherical topology incompatible with non-square map");
  }
}

