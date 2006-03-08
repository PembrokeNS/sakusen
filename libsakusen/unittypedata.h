#ifndef UNITTYPEDATA_H
#define UNITTYPEDATA_H

#include "iarchive.h"
#include "oarchive.h"
#include "iunittypedata.h"

namespace sakusen {

class UnitTypeData : public IUnitTypeData {
  private:
    UnitTypeData();
  public:
    UnitTypeData(
        HitPoints mHP,
        uint8 m,
        const Point<uint32>& s,
        const Region<sint16>& pA,
        const Region<sint16>& pV,
        const Region<sint16>& pAV,
        const Visibility& visib,
        const Sensors& visio
      );
    UnitTypeData(const IUnitTypeData* copy);

    HitPoints maxHitPoints;
    uint8 mass;
    Point<uint32> size;
    /* TODO: armour */
    Region<sint16> possibleAccelerations;
    Region<sint16> possibleVelocities;
    Region<sint16> possibleAngularVelocities;
    Visibility visibility;
    Sensors vision;
    
    inline HitPoints getMaxHitPoints(void) const { return maxHitPoints; }
    inline uint8 getMass(void) const {return mass;}
    inline const Point<uint32>& getSize(void) const {return size;}
    inline const Region<sint16>& getPossibleAccelerations() const {
      return possibleAccelerations;
    }
    inline const Region<sint16>& getPossibleVelocities() const {
      return possibleVelocities;
    }
    inline const Region<sint16>& getPossibleAngularVelocities() const {
      return possibleAngularVelocities;
    }
    inline const Visibility& getVisibility(void) const {return visibility;}
    inline const Sensors& getVision(void) const {return vision;}

    void store(OArchive&) const;
    static UnitTypeData load(IArchive&);
};

}

#endif // UNITTYPEDATA_H

