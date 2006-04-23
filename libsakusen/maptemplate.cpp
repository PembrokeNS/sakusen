#include "maptemplate.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

MapTemplate::MapTemplate(
    const Universe* u,
    const String& in,
    const Point<sint32>& tr,
    const Point<sint32>& bl,
    Topology t,
    const Heightfield& hf,
    uint16 g,
    const std::vector<MapPlayMode>& p
  ) :
  universe(u),
  internalName(in),
  topRight(tr),
  bottomLeft(bl),
  topology(t),
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



void MapTemplate::store(OArchive& archive) const
{
  archive.insertEnum(getTopology()) << internalName;
  archive << topRight << bottomLeft;
  heightfield.store(archive);
  archive << gravity << playModes;
}

MapTemplate MapTemplate::load(IArchive& archive, const Universe* universe)
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
  
  return MapTemplate(
      universe, internalName, topRight, bottomLeft,
      static_cast<Topology>(topology), heightfield, gravity, playModes
    );
}

