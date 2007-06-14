#ifndef UNITTYPE_H
#define UNITTYPE_H

#include "libsakusen-global.h"
#include "weapontype.h"
#include "unittypedata.h"

#include <list>

namespace sakusen {

class Universe;
class UnitType;

typedef const UnitType* UnitTypeId;

/** \brief Encapsulates everything about a particular type of unit.
 *
 * See \ref units for more details.
 */
class LIBSAKUSEN_API UnitType {
  private:
    UnitType();
  public:
    /** Constructs from all required data */
    UnitType(
      String internalName,
      const UnitTypeData& dynamicData,
      uint32 energyCost,
      uint32 metalCost,
      bool fixed,
      bool ground,
      bool surface,
      bool gravity,
      bool seabed,
      const std::vector<WeaponTypeId>& weapons,
      const UnitTypeId& corpseUnitType
    );
    /** \brief Constructs from most required data, with some details postponed
     *
     * This constructor is almost the same as the above constructor, but rather
     * than explicit WeaponTypeIds and UnitTypeIds, it takes Strings which will
     * be resolved into Ids later.
     *
     * \warning After calling this constructor, and before
     * using this UnitType, you *must* call UnitType::resolveNames to finish
     * the initilaization.
     */
    UnitType(
      String internalName,
      const UnitTypeData& dynamicData,
      uint32 energyCost,
      uint32 metalCost,
      bool fixed,
      bool ground,
      bool surface,
      bool gravity,
      bool seabed,
      const std::list<String>& weapons,
      const String& corpseUnitType
    );
    virtual ~UnitType() {}
  private:
    String internalName;
    UnitTypeData dynamicData;
    uint32 energyCost;
    uint32 metalCost;
    bool fixed:1; /* true for buildings, mines */
    bool ground:1; /* true for buildings, ground units, planes that can land
                      without a runway, hovercraft */
    bool surface:1; /* true for ships and floating buildings */
    bool gravity:1; /* false for planes, missiles, and such things which are
                       not affected by gravity */
    bool seabed:1; /* true for underwater metal mines, tanks that can drive on
                      the seabed */
    std::list<String> weaponNames;
    std::vector<WeaponTypeId> weapons;
    String corpseUnitTypeName;
    UnitTypeId corpseUnitType;
  public:
    String resolveNames(const Universe* universe);

    /** \name Accessors */
    //@{
    inline const String& getInternalName() const { return internalName; }
    inline const UnitTypeData& getDynamicData() const { return dynamicData; }
    inline const uint32& getEnergyCost() const { return energyCost; }
    inline const uint32& getMetalCost() const { return metalCost; }
    inline bool getGravity() const { return gravity; }
    inline const std::vector<WeaponTypeId>& getWeapons() const {
      return weapons;
    }
    inline UnitTypeId getCorpseUnitType() const { return corpseUnitType; }
    //@}
    
    void store(OArchive&) const;
    static UnitType load(IArchive&);
};

}

#endif
