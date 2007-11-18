#ifndef MAP_H
#define MAP_H

#include <vector>

#include "libsakusen-global.h"
#include "point.h"
#include "topology.h"
#include "iarchive.h"
#include "maptemplate.h"

namespace sakusen {

/** \brief Base class of all classes representing a map at game time
 *
 * Map is the base of a rather confusing class heirarchy which allows
 * representation of the map while the game is being played.  Inheriting
 * directly from Map are libsakuse::client::PartialMap and
 * libsakusen::server::CompleteMap, and then inheriting from each of those are
 * the various classes which define the map topology: PlaneMap, SphereMap etc.
 * The topology classes are templated so that they can inheruit from either of
 * the intermediate classes (and so that they can be defined in libsakusen
 * before either of the things they inherit from).
 *
 * Presumably there was a good reason for doing things this way but I can't
 * remember it.
 *
 * \todo Tiles */
class LIBSAKUSEN_API Map {
  private:
    Map();
  protected:
    Map(const Map&);
    Map(const MapTemplate&);
    Map(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity
      );
  public:
    virtual ~Map() {}
  private:
    Point<sint32> topRight; /* Excludes top right */
    Point<sint32> bottomLeft; /* Includes bottom left */
    uint16 gravity;
  public:
    /* accessors */
    virtual Topology getTopology(void) const = 0;
    virtual const IHeightfield& getHeightfield() const = 0;
    inline sint32 left(void) const { return bottomLeft.x; }
    inline sint32 right(void) const { return topRight.x; }
    inline sint32 bottom(void) const { return bottomLeft.y; }
    inline sint32 top(void) const { return topRight.y; }
    inline uint32 width(void) const { return right() - left(); }
    inline uint32 height(void) const { return top() - bottom(); }
    inline Rectangle<sint32> area(void) const {
      return Rectangle<sint32>(left(), bottom(), right(), top());
    }
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
    /** \brief Increments the given \a pos by \a inc, taking into account map
     * topology
     *
     * As above, but alters \a orientation as appropriate when moving over map
     * edges
     */
    inline Point<sint32> addToPosition(
        const Point<sint32>& pos, const Point<sint32>& inc,
        Orientation* orientation) const {
      Point<sint32> p;
      resolvePosition(pos+inc, &p, orientation);
      return p;
    }
    
    /** \brief Finds the shortest vector from op2 to op1
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
    
    template<typename MapType>
    static MapType* newMap(const MapTemplate& t);
    
    template<typename MapType>
    static MapType* newMap(
        Topology topology,
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield&
      );
};

}

#endif // MAP_H

