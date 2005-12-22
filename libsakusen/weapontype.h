#ifndef WEAPONTYPE_H
#define WEAPONTYPE_H

#include "libsakusen-global.h"

class Weapon;
class WeaponType;

typedef const WeaponType* WeaponTypeID;

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <list>
#pragma warning (disable: 4231)
EXPORT_LIST(WeaponTypeID)
#endif

class WeaponType {
  friend class Weapon;
  private:
    String internalName;
    uint16 energyCost;
    uint16 metalCost;
    uint16 energyRate;
    uint16 metalRate;

  protected:
    WeaponType();
    WeaponType(const WeaponType&);
    public:
    virtual ~WeaponType();

    /* factory method */
    /* this creates a new weapon which must be deleted */
    virtual Weapon& spawnWeapon(void);
    /* accessors */
    inline const String& getInternalName(void) const { return internalName; }
    inline WeaponTypeID getId(void) const {return this;}
    inline uint16 getEnergyCost(void) const {return energyCost;}
    inline uint16 getMetalCost(void) const {return metalCost;}
    inline uint32 getCost(void) const {return (energyCost<<16)|metalCost;}
    inline uint16 getEnergyRate(void) const {return energyRate;}
    inline uint16 getMetalRate(void) const {return metalRate;}
    inline uint32 getRate(void) const {return (energyRate<<16)|metalRate;}

};

#endif
