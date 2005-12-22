#ifndef PLANEMAP_H
#define PLANEMAP_H

#include "map.h"

class PlaneMap : public Map {
  private:
    PlaneMap();
  public:
    PlaneMap(
        const Universe* universe,
        const String& name,
        const Point<uint32>& volume,
        const Heightfield& heightfield,
        uint16 gravity,
        const std::vector<MapPlayMode>& playModes
      );
    PlaneMap(
        const Universe* universe,
        const String& name,
        const Point<uint32>& topRight,
        const Point<uint32>& bottomLeft,
        const Heightfield& heightfield,
        uint16 gravity,
        const std::vector<MapPlayMode>& playModes
      );

    Topology getTopology(void) const { return topology_plane; }
    bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>* resolved,
        Orientation* orientation
      ) const;
    Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const;
};

#endif // PLANEMAP_H

