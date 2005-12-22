#ifndef SPHEREMAP_H
#define SPHEREMAP_H

#include "map.h"

class SphereMap : public Map {
  private:
    SphereMap();
  public:
    SphereMap(
        const Universe* universe,
        const String& name,
        const Point<uint32>& volume,
        const Heightfield& heightfield,
        uint16 gravity,
        const std::vector<MapPlayMode>& playModes
      );
};

#endif // SPHEREMAP_H

