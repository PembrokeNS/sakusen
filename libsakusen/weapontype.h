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

typedef const WeaponType* WeaponTypeId;

class LIBSAKUSEN_API WeaponType {
  private:
    WeaponType();
  public:
    WeaponType(
        String internalName,
        String moduleName,
        String clientHint,
        uint16 energyCost,
        uint16 metalCost,
        uint16 energyRate,
        uint16 metalRate,
        ResourceInterface::Ptr resourceInterface
      );
  private:
    /** \brief Internal name of the WeaponType
     *
     * Used for serialization and to determine the name of the spawn function
     */
    String internalName;
    /** \brief Name of module containing this WeaponType's spawn function */
    String moduleName;
    /** \brief Hint for clients as to the purpose of the weapon
     *
     * This is so that clients can have an inkling of what weapons do, even
     * when they have not been manually customized to a particular universe.
     * We might want to think about some kind of sensible syntax, but for now
     * I'm throwing in the following:
     * \list
     * \li o &mdash; offensive weapon.
     * \li c:foo &mdash; creates unit type foo (internal name).
     * \li b &mdash; for building of things after they've been created.
     * \endlist
     * Clients should certainly accept any value in this field without choking.
     */
    String clientHint;
    server::Weapon* (*spawnFunction)(const WeaponType*);
    uint16 energyCost;
    uint16 metalCost;
    uint16 energyRate;
    uint16 metalRate;
  public:
    /** \name Accessors. */
    //@{
    inline const String& getInternalName(void) const { return internalName; }
    inline const String& getClientHint() const { return clientHint; }
    inline WeaponTypeId getId(void) const {return this;}
    inline uint16 getEnergyCost(void) const {return energyCost;}
    inline uint16 getMetalCost(void) const {return metalCost;}
    inline uint32 getCost(void) const {return (energyCost<<16)|metalCost;}
    inline uint16 getEnergyRate(void) const {return energyRate;}
    inline uint16 getMetalRate(void) const {return metalRate;}
    inline uint32 getRate(void) const {return (energyRate<<16)|metalRate;}
    //@}

    server::Weapon* spawn() const;

    void store(OArchive&) const;
    static WeaponType load(IArchive&, const DeserializationContext&);
};

}

#endif

