#include "maptemplate.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

MapTemplate::MapTemplate(
    const Universe::ConstPtr& u,
    const String& in,
    const Point<sint32>& tr,
    const Point<sint32>& bl,
    Topology t,
    const Heightfield& hf,
    uint32 d,
    uint16 g,
    const std::vector<MapPlayMode>& p
  ) :
  universe(u),
  internalName(in),
  topRight(tr),
  bottomLeft(bl),
  topology(t),
  heightfield(hf),
  dexPerPixel(d),
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
  archive << dexPerPixel << gravity << playModes;
}

MapTemplate MapTemplate::load(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  Topology topology;
  String internalName;
  Point<sint32> topRight;
  Point<sint32> bottomLeft;
  archive.extractEnum(topology) >> internalName >> topRight >> bottomLeft;
  Heightfield heightfield = Heightfield::load(archive, context);
  uint32 dexPerPixel;
  uint16 gravity;
  vector<MapPlayMode> playModes;
  archive >> dexPerPixel;
  archive >> gravity;
  archive.extract(playModes, context);
  
  return MapTemplate(
      context.getUniverse(), internalName, topRight, bottomLeft,
      topology, heightfield, dexPerPixel, gravity, playModes
    );
}

