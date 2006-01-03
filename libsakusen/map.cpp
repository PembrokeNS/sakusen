#include "map.h"

#include "planemap.h"
#include "iarchive-methods.h"
#include "oarchive-methods.h"

using namespace std;

Map::Map(
    const Universe* u,
    const String& in,
    const Point<uint32>& tr,
    const Point<uint32>& bl,
    const Heightfield& hf,
    uint16 g,
    const std::vector<MapPlayMode>& p
  ) :
  universe(u),
  internalName(in),
  topRight(tr),
  bottomLeft(bl),
  heightfield(hf),
  gravity(g),
  playModes(p)
{
  /* NB: order on Points is *partial*, so the following not equivalent to
   * topRight < bottomLeft
   *
   * We allow maps of zero area because they are actually useful in some
   * circumstances (e.g. a map which exists only for its briefing) */
  if (!(topRight >= bottomLeft)) {
    Fatal("map has negative dimension");
  }
  
  /* TODO: determine number of distance units per pixel of heightfield, and
   * ensure it is sane (in particular, that it is an integer, possibly even
   * that it is a power of 2 (so we can do conversion by bitshifting)) */
  
  for (std::vector<MapPlayMode>::iterator playMode = playModes.begin();
      playMode != playModes.end(); playMode++) {
    if (playMode->sanityCheck(universe, *this)) {
      Fatal("MapPlayMode sanity check failed");
    }
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

void Map::store(OArchive& archive) const
{
  archive << uint8(getTopology()) << internalName;
  archive << topRight << bottomLeft;
  heightfield.store(archive);
  archive << gravity << playModes;
}

Map* Map::loadNew(IArchive& archive, const Universe* universe)
{
  uint8 topology;
  String internalName;
  Point<sint32> topRight;
  Point<sint32> bottomLeft;
  archive >> topology >> internalName >> topRight >> bottomLeft;
  Heightfield heightfield = Heightfield::load(archive);
  uint16 gravity;
  vector<MapPlayMode> playModes;
  archive >> gravity;
  archive.extract(playModes, universe);
  
  switch(topology) {
    case topology_plane:
      return new PlaneMap(
          universe, internalName, topRight, bottomLeft, heightfield, gravity,
          playModes
        );
    default:
      /* FIXME: should be a thrown exception, not a fatal error */
      Fatal("unepected topology: " << topology);
  }
}

