#ifndef MAPTEMPLATE_H
#define MAPTEMPLATE_H

#include "libsakusen-global.h"
#include "universe.h"
#include "heightfield.h"
#include "mapplaymode.h"
#include "topology.h"

namespace sakusen {

/** \brief Defines a map and one or more ways to play a game on it.
 *
 * A MapTemplate defines the physical properties of one map (heightfield, water
 * level, gravity, etc.) and one or more MapPlayModes which can turn this map
 * into a playable game by specifying what players exist, what units they have,
 * etc.
 *
 * \todo Tiles.
 */
class LIBSAKUSEN_API MapTemplate {
  public:
    typedef boost::shared_ptr<MapTemplate> Ptr;
    typedef boost::shared_ptr<const MapTemplate> ConstPtr;

    MapTemplate(
        const Universe::ConstPtr& universe,
        const String& internalName,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        Topology topology,
        const Heightfield& heightfield,
        uint16 gravity,
        const std::vector<MapPlayMode>& playModes
      );
  private:
    Universe::ConstPtr universe;
    String internalName;
    Point<sint32> topRight; /* Excludes top right */
    Point<sint32> bottomLeft; /* Includes bottom left */
    Topology topology;
    Heightfield heightfield;
    uint16 gravity;
    std::vector<MapPlayMode> playModes;
  public:
    /* accessors */
    inline const Universe::ConstPtr& getUniverse(void) const {
      return universe;
    }
    inline const String& getInternalName(void) const { return internalName; }
    /** \brief Get the triple of minima for the coordinate space occupied by
     * the map (included). */
    inline const Point<sint32>& getBottomLeft(void) const { return bottomLeft; }
    /** \brief Get the triple of maxima for the coordinate space occupied by
     * the map (excluded). */
    inline const Point<sint32>& getTopRight(void) const { return topRight; }
    /** \brief Determine whether a position is within the bounds of the map */
    inline bool containsPoint(Point<sint32>  p) const
    {
      return bottomLeft <= p && p < topRight;
    }
    inline Topology getTopology(void) const { return topology; }
    inline const Heightfield& getHeightfield(void) const { return heightfield; }
    inline uint16 getGravity(void) const { return gravity; }
    inline const std::vector<MapPlayMode>& getPlayModes(void) const {
      return playModes;
    }

    typedef Universe::ConstPtr loadArgument;
    void store(OArchive& archive) const;
    static MapTemplate load(IArchive& archive, const loadArgument* universe);
};

}

#endif

