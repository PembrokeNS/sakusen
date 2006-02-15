#ifndef PARTIALMAP_H
#define PARTIALMAP_H

namespace sakusen {
namespace client {

class LIBSAKUSEN_API PartialMap : public Map {
  private:
    PartialMap();
  public:
    PartialMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity
      ) : Map(topRight, bottomLeft, gravity) {}
};

}}

#endif // PARTIALMAP_H

