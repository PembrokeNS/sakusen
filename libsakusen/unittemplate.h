#ifndef UNITTEMPLATE_H
#define UNITTEMPLATE_H

#include "libsakusen-global.h"

#include "point.h"
#include "unittype.h"
#include "orientation.h"
#include "universe.h"

namespace sakusen {

class LIBSAKUSEN_API UnitTemplate {
  private:
    UnitTemplate();
  public:
    UnitTemplate(
      const Universe* universe,
      const UnitTypeID& type,
      const Point<sint32>& position,
      const Orientation& orientation,
      const Point<sint16>& velocity,
      HitPoints hitPoints,
      bool radarIsActive,
      bool sonarIsActive
    );
  private:
    const Universe* universe;
      /* a 'private line' to the universe is needed because there's no world
       * when UnitTemplates are being used */
    UnitTypeID type;
    Point<sint32> position; /* this maybe needs a 'magic value' for when the
                               unit is a subunit of another unit */
    Orientation orientation;
    Point<sint16> velocity; /* velocity is in distance-units per frame */
    
    /* status stuff */
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    std::list<UnitTemplate*> subunits;
      /* TODO: think hard about this.  What happens
				 when the subunits belong to another player?
				 What happens when this Unit is serialized? */

  public:
    /* accessors */
    inline UnitTypeID getType(void) const {return type;}
    inline const Point<sint32>& getPosition(void) const {return position;}
    void setPosition(const Point<sint32> pos);
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline void setOrientation(const Orientation& o) { orientation = o; }
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline void setVelocity(const Point<sint16>& v) { velocity = v; }
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    bool setRadarActive(bool active);
    inline bool isSonarActive(void) const {return sonarIsActive;}
    bool setSonarActive(bool active);
    
    typedef Universe loadArgument;
    void store(OArchive&) const;
    static UnitTemplate load(IArchive&, const loadArgument*);
};
}

#endif // UNITTEMPLATE_H

