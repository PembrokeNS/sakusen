#ifndef LIBSAKUSEN__UNITTYPEDATA_H
#define LIBSAKUSEN__UNITTYPEDATA_H

#include "iarchive.h"
#include "oarchive.h"
#include "iunittypedata.h"

namespace sakusen {

/** \brief Defines a variety of unit for a Universe
 *
 * A UnitType contains all the information about a variety of unit but not
 * anything about any particular instance of that variety.
 *
 * \todo Armour?
 */
class LIBSAKUSEN_API UnitTypeData : public IUnitTypeData {
  private:
    UnitTypeData();
  public:
    UnitTypeData(
        HitPoints mHP,
        uint8 m,
        const Point<uint32>& s,
        const Region<sint16>::Ptr& pA,
        const Region<sint16>::Ptr& pV,
        const Region<sint32>::Ptr& pAV,
        const Visibility& visib,
        const Sensors& visio
      );
    UnitTypeData(const IUnitTypeData& copy);

    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    Region<sint16>::ConstPtr possibleAccelerations;
    Region<sint16>::ConstPtr possibleVelocities;
    Region<sint32>::ConstPtr possibleAngularVelocities;
    Visibility visibility;
    Sensors vision;
    
    /** \name Accessors */
    //@{
    inline HitPoints getMaxHitPoints(void) const { return maxHitPoints; }
    inline uint8 getMass(void) const {return mass;}
    inline const Point<uint32>& getSize(void) const {return size;}
    inline Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return possibleAccelerations;
    }
    inline Region<sint16>::ConstPtr getPossibleVelocities() const {
      return possibleVelocities;
    }
    inline Region<sint32>::ConstPtr getPossibleAngularVelocities() const {
      return possibleAngularVelocities;
    }
    inline const Visibility& getVisibility(void) const {return visibility;}
    inline const Sensors& getVision(void) const {return vision;}
    //@}

    void store(OArchive&) const;
    static UnitTypeData load(IArchive&);
};

}

#endif // LIBSAKUSEN__UNITTYPEDATA_H

