#ifndef COMPLETEMAP_H
#define COMPLETEMAP_H

#include "libsakusen-server-global.h"
#include "map.h"
#include "heightfield.h"
#include "planemap.h"

namespace sakusen {
namespace server {

class LIBSAKUSEN_SERVER_API CompleteMap : public Map {
  private:
    CompleteMap();
  protected:
    CompleteMap(const Map&);
    CompleteMap(const MapTemplate&);
    CompleteMap(
        const Point<sint32>& topRight,
        const Point<sint32>& bottomLeft,
        uint16 gravity,
        const Heightfield& heightfield
      );
  public:
    virtual ~CompleteMap() {}
  private:
    Heightfield heightfield;

    void sanityCheck();
  public:
    inline const Heightfield& getHeightfield() const { return heightfield; }
    static Map* newMap(const MapTemplate& t);
};

#ifdef _MSC_VER
template LIBSAKUSEN_SERVER_API class PlaneMap<CompleteMap>;
#endif

}}

#endif // COMPLETEMAP_H

