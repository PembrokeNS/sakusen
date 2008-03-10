#ifndef COMPLETEMAP_H
#define COMPLETEMAP_H

#include "libsakusen-server-global.h"
#include "map.h"
#include "heightfield.h"

namespace sakusen {
namespace server {

/** \brief Represents the complete state of the game map.
 *
 * This class carries all the physical information about the map, such as the
 * heightfield and gravity.  Being on server side, it knows everything there is
 * to know about the map.
 */
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
        uint32 xyHeightfieldRes,
        uint32 zHeightfieldRes
      );
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

}}

#endif // COMPLETEMAP_H

