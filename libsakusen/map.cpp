#include "map.h"

using namespace std;

using namespace sakusen;

/* I assumed this function was supposed to be a copy constructor, 
 * auto-generated by something or other? In order to export some 
 * of the classes derived from Map, I had to make it explicit, 
 * I hope this was what it was supposed to be. */
 
Map::Map(const Map& copy)
{
  *this = copy;
}

Map::Map(const MapTemplate& t) :
  topRight(t.getTopRight()),
  bottomLeft(t.getBottomLeft()),
  gravity(t.getGravity())
{
  /* NB: order on Points is *partial*, so the following not equivalent to
   * topRight < bottomLeft
   *
   * We allow maps of zero area because they are actually useful in some
   * circumstances (e.g. a map which exists only for its briefing) */
  if (!(topRight >= bottomLeft)) {
    Fatal("map has negative dimension");
  }
}

Map::Map(
    const Point<sint32>& tR,
    const Point<sint32>& bL,
    uint16 g
  ) :
  topRight(tR),
  bottomLeft(bL),
  gravity(g)
{
  if (!(topRight >= bottomLeft)) {
    Fatal("map has negative dimension");
  }
}

/* The following function commented out but left so that the code can be
 * stolen when the remaining *Map classes are written */
#if 0
bool Map::resolvePosition(
    const Point<sint32>& pos, Point<sint32>* resolved,
    Orientation* orientation) const
{
  Point <sint32> p = pos;
  
  /* Here's where we endeavour to actually implement our multitudinous array of
   * topologies.  However, this function is relatively trivial compared to
   * getShortestDifference below */
  bool altered;
  bool truncated = false;
  sint32 newX;
  sint32 newY;
  
  do {
    altered = false;

    if (p.x<left()) {
      altered = true;
      switch (topology) {
        case topology_cylinder:
        case topology_torus:
        case topology_klein:
          p.x += width();
          break;
        case topology_mobius:
        case topology_crosscut:
          p.x += width();
          p.y = reflectY(p.y);
          if (orientation != NULL) {
            *orientation = Orientation(reflection_y) * *orientation;
          }
          break;
        case topology_sphere:
          newY = top() - (left() - p.x);
          p.x = left() + (top() - p.y - 1);
          p.y = newY;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 90) * *orientation;
          }
          break;
        default:
          Fatal("unknown topology");
          break;
      }
    }

    if (p.x>=right()) {
      altered = true;
      switch (topology) {
        case topology_cylinder:
        case topology_torus:
        case topology_klein:
          p.x -= width();
          break;
        case topology_mobius:
        case topology_crosscut:
          p.x -= width();
          p.y = reflectY(p.y);
          if (orientation != NULL) {
            *orientation = Orientation(reflection_y) * *orientation;
          }
          break;
        case topology_sphere:
          newY = bottom() + (p.x - right());
          p.x = left() + (top() - p.y - 1);
          p.y = newY;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise,90) * *orientation;
          }
          break;
        default:
          Fatal("unknown topology");
          break;
      }
    }

    if (p.y<bottom()) {
      altered = true;
      switch (topology) {
        case topology_cylinder:
        case topology_mobius:
          p.y = bottom();
          truncated = true;
          break;
        case topology_torus:
          p.y += height();
          break;
        case topology_klein:
        case topology_crosscut:
          p.y += height();
          p.x = reflectX(p.x);
          break;
        case topology_sphere:
          newX = right() - (bottom() - p.y);
          p.y = bottom() + (right() - p.x - 1);
          p.x = newX;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 270) * *orientation;
          }
          break;
        default:
          Fatal("unknown topology");
          break;
      }
    }

    if (p.y>=top()) {
      altered = true;
      switch (topology) {
        case topology_cylinder:
        case topology_mobius:
          p.y = top() - 1;
          truncated = true;
          break;
        case topology_torus:
          p.y -= height();
          break;
        case topology_klein:
        case topology_crosscut:
          p.y -= height();
          p.x = reflectX(p.x);
          break;
        case topology_sphere:
          newX = left() + (p.y - bottom());
          p.y = bottom() + (right() - p.x - 1);
          p.x = newX;
          if (orientation != NULL) {
            *orientation =
              Orientation(rotation_anticlockwise, 270) * *orientation;
          }
          break;
        default:
          Fatal("unknown topology");
          break;
      }
    }
  } while (altered);

  if (resolved != NULL) {
    *resolved = p;
  }
  
  return truncated;
}
#endif

