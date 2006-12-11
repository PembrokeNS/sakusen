#ifndef PARTIALMAP_H
#define PARTIALMAP_H

#include "libsakusen-client-global.h"
#include "planemap.h"
#include "partialheightfield.h"

namespace sakusen {
namespace client {

class LIBSAKUSEN_CLIENT_API PartialMap : public Map {
  public:
    PartialMap(const MapTemplate& t) :
        Map(t),
        heightfield(t.getHeightfield())
    {
      sanityCheck();
    }
    PartialMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        uint32 horizontalHeightfieldRes,
        uint32 verticalHeightfieldRes
      ) :
      Map(topRight, bottomLeft, gravity),
      heightfield(
          horizontalHeightfieldRes,
          verticalHeightfieldRes,
          1 + width() / horizontalHeightfieldRes,
          1 + height() / verticalHeightfieldRes
        )
    {
      sanityCheck();
    }
  private:
    PartialHeightfield heightfield;

    void sanityCheck();
  public:
    const IHeightfield& getHeightfield() const {
      return heightfield;
    }
};

#ifdef _MSC_VER
template LIBSAKUSEN_CLIENT_API class PlaneMap<PartialMap>;
#endif

}}

#endif // PARTIALMAP_H

