#ifndef WEAPONTYPE_H
#define WEAPONTYPE_H

#include "libsakusen-global.h"
#include "iarchive.h"
#include "oarchive.h"
#include "resourceinterface.h"

namespace sakusen {

namespace server {
/* Sticking a declaration of sakusen::server::Weapon in here is a bit cheap,
 * but it allows us to use that type as the return type of the spawn
 * member.  The alternative would be to have it return a void* and scatter
 * reinterpret_casts everywhere.  The member in question should never be used
 * on client-side. */
class Weapon;
}

class WeaponType;

typedef const WeaponType* WeaponTypeID;

class LIBSAKUSEN_API WeaponType {
  private:
    WeaponType();
  public:
    WeaponType(
        String internalName,
        String moduleName,
        uint16 energyCost,
        uint16 metalCost,
        uint16 energyRate,
        uint16 metalRate,
        ResourceInterface* resourceInterface
      );
  private:
    String internalName;
    String moduleName;
    server::Weapon* (*spawnFunction)(const WeaponType*);
    uint16 energyCost;
    uint16 metalCost;
    uint16 energyRate;
    uint16 metalRate;
  public:
    /* accessors */
    inline const String& getInternalName(void) const { return internalName; }
    inline WeaponTypeID getId(void) const {return this;}
    inline uint16 getEnergyCost(void) const {return energyCost;}
    inline uint16 getMetalCost(void) const {return metalCost;}
    inline uint32 getCost(void) const {return (energyCost<<16)|metalCost;}
    inline uint16 getEnergyRate(void) const {return energyRate;}
    inline uint16 getMetalRate(void) const {return metalRate;}
    inline uint32 getRate(void) const {return (energyRate<<16)|metalRate;}

    server::Weapon* spawn() const;

    void store(OArchive&) const;
    static WeaponType load(IArchive&, ResourceInterface*);
};

}

#endif

