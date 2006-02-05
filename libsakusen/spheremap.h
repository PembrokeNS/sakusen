#ifndef SPHEREMAP_H
#define SPHEREMAP_H

#include "map.h"

namespace sakusen {

class SphereMap : public Map {
  private:
    SphereMap();
  public:
    SphereMap(const MapTemplate&);
};

}

#endif // SPHEREMAP_H

