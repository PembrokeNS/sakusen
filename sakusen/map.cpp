#include <sakusen/map.h>

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
    SAKUSEN_FATAL("map has negative dimension");
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
    SAKUSEN_FATAL("map has negative dimension");
  }
}

