#ifndef LIBSAKUSEN__UNITSTATUS_H
#define LIBSAKUSEN__UNITSTATUS_H

#include "libsakusen-global.h"

#include "iunitstatus.h"
#include "universe.h"

namespace sakusen {

/** \brief Encapsulates status information about a unit.
 *
 * See \ref units for more details.
 */
class LIBSAKUSEN_API UnitStatus : public IUnitStatus {
  private:
    UnitStatus(); /**< Default constructor should not be used */
    UnitStatus(
        UnitTypeID t,
        const Point<sint32>& p,
        const Orientation& o,
        const Point<sint16>& v,
        /* status stuff */
        HitPoints hP,
        bool rIA,
        bool sIA,
        const std::list<uint32>& su,
        const std::vector<WeaponStatus>& w
      );
  public:
    UnitStatus(const IUnitStatus* copy);
    UnitStatus(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      HitPoints startHitPoints,
      bool startRadarActive,
      bool startSonarActive,
      std::vector<WeaponStatus> startWeaponsStatus
    );
    UnitStatus(
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity,
      const HitPoints startHP
    ); /**< This is a simpler version of the above constructor for when you
         don't need to specify the extra data */
    UnitStatus(
      const Universe::ConstPtr& universe,
      const UnitTypeID& startType,
      const Point<sint32>& startPosition,
      const Orientation& startOrientation,
      const Point<sint16>& startVelocity
    ); /**< This is a variant of the above constructor for when the world does
          not exist yet, so the universe must be provided directly */
  public:
    UnitTypeID type;
    /** \name Unit's physical attributes */
    //@{
    Position position;
    Orientation orientation;
    Velocity velocity;
    //@}
    
    /** \name Unit's status */
    //@{
    HitPoints hitPoints;
    bool radarIsActive;
    bool sonarIsActive;
    //@}
    /** \brief List of subunits
     *
     * \note Subunits which must belong to the same player as this unit, so
     * need only specify ids. */
    std::list<uint32> subunits;
    std::vector<WeaponStatus> weaponsStatus;

    void initializeWeapons(const UnitType* typePtr);
  public:
    /* accessors */
    inline UnitTypeID getType(void) const {return type;}
    inline const UnitType* getTypePtr(void) const;
    inline const Point<sint32>& getPosition(void) const {return position;}
    inline const Orientation& getOrientation(void) const {return orientation;}
    inline const Point<sint16>& getVelocity(void) const {return velocity;}
    inline HitPoints getHitPoints(void) const {return hitPoints;}
    inline bool isRadarActive(void) const {return radarIsActive;}
    inline bool isSonarActive(void) const {return sonarIsActive;}
    inline const std::vector<WeaponStatus>& getWeaponsStatus(void) const {
      return weaponsStatus;
    }
    inline std::vector<WeaponStatus>& getWeaponsStatus(void) {
      return weaponsStatus;
    }

    typedef Universe::ConstPtr loadArgument;
    void store(OArchive&, Universe::ConstPtr const&) const;
    static UnitStatus load(IArchive&, const loadArgument*);
};

}

#endif // LIBSAKUSEN__UNITSTATUS_H

