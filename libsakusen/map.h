#ifndef MAP_H
#define MAP_H

#include <vector>

#include "libsakusen-global.h"
#include "point.h"
#include "topology.h"
#include "heightfield.h"
#include "mapplaymode.h"
#include "universe.h"
#include "iarchive.h"

namespace sakusen {

class Map;

class LIBSAKUSEN_API Map {
  private:
    Map();
  protected:
    Map(const Map&);
    Map(const Universe* universe,
        const String& internalName,
        const Point<uint32>& topRight,
        const Point<uint32>& bottomLeft,
        const Heightfield& heightfield,
        uint16 gravity,
        const std::vector<MapPlayMode>& playModes
      );
  public:
    virtual ~Map() {}
  private:
    const Universe* universe; /* Not owned by this */
    String internalName;
    Point<sint32> topRight; /* Excludes top right */
    Point<sint32> bottomLeft; /* Includes bottom left */
    Heightfield heightfield;
    /* TODO: tiles */
    uint16 gravity;
    std::vector<MapPlayMode> playModes;
  public:
    /* accessors */
    virtual Topology getTopology(void) const = 0;
    inline const Universe* getUniverse(void) const {
      return universe;
    }
    inline const String& getInternalName(void) const { return internalName; }
    inline sint32 left(void) const { return bottomLeft.x; }
    inline sint32 right(void) const { return topRight.x; }
    inline sint32 bottom(void) const { return bottomLeft.y; }
    inline sint32 top(void) const { return topRight.y; }
    inline uint32 width(void) const { return right() - left(); }
    inline uint32 height(void) const { return top() - bottom(); }
    inline sint32 reflectX(sint32 x) const
    {
      return right() - (x - left() + 1);
    }
    inline sint32 reflectY(sint32 y) const
    {
      return top() - (y - bottom() + 1);
    }
    inline bool containsPoint(Point<sint32>  p) const
    {
      return bottomLeft <= p && p < topRight;
    }
    
    inline uint16 getGravity(void) const { return gravity; }
    inline const MapPlayMode& getPlayMode(uint32 i) const {
      assert(i<playModes.size());
      return playModes[i];
    }

    /* game mechanics */
    
    /** \brief Resolves any point in space into one within the bounds of the
     * map
     * 
     * Takes the position \a pos, and adjusts it (through wrapping and
     * truncation) to force it into the map
     * area, returning the adjusted value through resolved.  Returns true
     * iff truncation was necessary.  Note that truncation means exactly that -
     * it will not return true simply because it wrapped over a map edge.
     * 
     * The \a orientation argument (if not \c NULL)
     * is updated by prepending rotations and
     * reflections as appropriate when transitioning over map edges.
     *
     * The z-coordinate of pos is left unchanged in resolved.
     */
    virtual bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>* resolved,
        Orientation* orientation
      ) const = 0;
    /** \brief Resolves any point in space into one within the bounds of the
     * map
     *
     * As above, but without the orientation argument.  Appropriate, for
     * example, for use with Ballistics, which have no orientation
     */
    inline bool resolvePosition(
        const Point<sint32>& pos, Point<sint32>* resolved) const {
      return resolvePosition(pos, resolved, NULL);
    }
    
    /** \brief Increments the given \a pos by \a inc, taking into account map
     * topology
     * 
     * This method guaranteed to yield a point inside the map, even at the
     * cost of truncation
     */
    inline Point<sint32> addToPosition(
        const Point<sint32>& pos, const Point<sint32>& inc) const {
      return addToPosition(pos, inc, NULL);
    }
    inline Point<sint32> addToPosition(
        const Point<sint32>& pos, const Point<sint16>& inc) const {
      return addToPosition(pos, Point<sint32>(inc));
    }
    inline Point<sint32> addToPosition(
        const Point<sint32>& pos, const Point<sint32>& inc,
        Orientation* orientation) const {
      Point<sint32> p;
      resolvePosition(pos+inc, &p, orientation);
      return p;
    }
    
    /* \brief Increments the given \a pos by \a inc, taking into account map
     * topology
     *
     * As above, but alters \a orientation as appropriate when moving over map
     * edges
     */
    inline Point<sint32> addToPosition(
        const Point<sint32>& pos, const Point<sint16>& inc,
        Orientation* orientation) const {
      return addToPosition(pos, Point<sint32>(inc), orientation);
    }
    
    /* \brief Finds the shortest vector from op2 to op1
     *
     * Intuitively, this function returns \a op1 - \a op2.
     * Technically, it returns the shortest vector \c x such that
     * \c addToPosition(op2,x)=op1
     *
     * \todo We need to clearly define what happens in cases of ambiguity
     */
    virtual Point<sint32> getShortestDifference(
        const Point<sint32>& op1, const Point<sint32>& op2
      ) const = 0;

    typedef Universe loadArgument;
    void store(OArchive& archive) const;
    static Map* loadNew(IArchive& archive, const loadArgument* universe);
};

}

#endif // MAP_H

