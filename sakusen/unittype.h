#ifndef LIBSAKUSEN__UNITTYPE_H
#define LIBSAKUSEN__UNITTYPE_H

#include <sakusen/global.h>
#include <sakusen/weapontype.h>
#include <sakusen/unittypedata.h>

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
      const String& internalName,
      const UnitTypeData& dynamicData,
      uint32 energyCost,
      uint32 metalCost,
      const String& motionType,
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
     * using this UnitType, you \e must call UnitType::resolveNames to finish
     * the initilaization.
     */
    UnitType(
      const String& internalName,
      const UnitTypeData& dynamicData,
      uint32 energyCost,
      uint32 metalCost,
      const String& motionType,
      const std::list<String>& weapons,
      const String& corpseUnitType
    );
    virtual ~UnitType() {}
  private:
    String internalName;
    UnitTypeData dynamicData;
    uint32 energyCost;
    uint32 metalCost;
    String motionType;
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
    inline const String& getMotionType() const { return motionType; }
    inline const std::vector<WeaponTypeId>& getWeapons() const {
      return weapons;
    }
    inline UnitTypeId getCorpseUnitType() const { return corpseUnitType; }
    //@}

    void store(OArchive&) const;
    static UnitType load(IArchive&);
};

}

#endif // LIBSAKUSEN__UNITTYPE_H

