#ifndef PARTIALMAP_H
#define PARTIALMAP_H

#include "libsakusen-client-global.h"
#include "planemap.h"

namespace sakusen {
namespace client {

class LIBSAKUSEN_CLIENT_API PartialMap : public Map {
  private:
    PartialMap();
  public:
    PartialMap(const MapTemplate& t) :
        Map(t) {}
    PartialMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity
      ) : Map(topRight, bottomLeft, gravity) {}
};

#ifdef _MSC_VER
template LIBSAKUSEN_CLIENT_API class PlaneMap<PartialMap>;
#endif

}}

#endif // PARTIALMAP_H

