#ifndef PLANEMAP_H
#define PLANEMAP_H

#include "map.h"

namespace sakusen {

class LIBSAKUSEN_API PlaneMap : public Map {
  private:
    PlaneMap();
  public:
    PlaneMap(const MapTemplate& t);

    Topology getTopology(void) const { return topology_plane; }
    bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>* resolved,
        Orientation* orientation
      ) const;
    Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const;
};

}

#endif // PLANEMAP_H

