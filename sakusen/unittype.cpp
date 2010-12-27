#include <sakusen/unittype.h>

#include <sakusen/universe.h>
#include <sakusen/oarchive-methods.h>

using namespace sakusen;

UnitType::UnitType(
    const String& iN,
    const UnitTypeData& dD,
    uint32 eC,
    uint32 mC,
    const String& mT,
    const std::vector<WeaponTypeId>& w,
    const UnitTypeId& cUT) :
  internalName(iN),
  dynamicData(dD),
  energyCost(eC),
  metalCost(mC),
  motionType(mT),
  weapons(w),
  corpseUnitType(cUT)
{
}

UnitType::UnitType(
    const String& iN,
    const UnitTypeData& dD,
    uint32 eC,
    uint32 mC,
    const String& mT,
    const std::list<String>& w,
    const String& cUT) :
  internalName(iN),
  dynamicData(dD),
  energyCost(eC),
  metalCost(mC),
  motionType(mT),
  weaponNames(w),
  corpseUnitTypeName(cUT),
  corpseUnitType(NULL)
{
}

/** \brief Converts all the weapon type names into proper WeaponTypeIds by
 * querying the given Universe */
String UnitType::resolveNames(const Universe* universe)
{
  assert(weapons.empty());
  assert(corpseUnitType == NULL);
  
  while (!weaponNames.empty()) {
    WeaponTypeId weaponId = universe->getWeaponTypeId(weaponNames.front());
    if (weaponId == NULL) {
      return weaponNames.front();
    }
    weaponNames.pop_front();
    weapons.push_back(weaponId);
  }

  if (corpseUnitTypeName != "") {
    UnitTypeId corpseId = universe->getUnitTypeId(corpseUnitTypeName);
    if (corpseId == NULL) {
      return corpseUnitTypeName;
    }
    corpseUnitTypeName = "";
    corpseUnitType = corpseId;
  } else {
    corpseUnitType = NULL;
  }

  return "";
}

void UnitType::store(OArchive& archive) const
{
  archive << internalName;
  dynamicData.store(archive);
  archive << energyCost << metalCost;
  archive << motionType << weapons <<
    (corpseUnitType == NULL ? String("") : corpseUnitType->getInternalName());
    /** \bug assumes UnitTypeId is UnitType* */
}

UnitType UnitType::load(IArchive& archive)
{
  String internalName;
  archive >> internalName;
  UnitTypeData dynamicData = UnitTypeData::load(archive);
  uint32 energyCost;
  uint32 metalCost;
  archive >> energyCost >> metalCost;
  String motionType;
  std::list<String> weapons;
  String corpseUnitType;
  archive >> motionType >> weapons >> corpseUnitType;

  return UnitType(
      internalName, dynamicData, energyCost, metalCost, motionType,
      weapons, corpseUnitType
    );
}

