#include "unittype.h"

#include "universe.h"
#include "oarchive-methods.h"

using namespace sakusen;

UnitType::UnitType(
    String iN,
    const UnitTypeData& dD,
    uint32 eC,
    uint32 mC,
    bool f,
    bool gro,
    bool su,
    bool gra,
    bool se,
    const std::vector<WeaponTypeID>& w,
    const UnitTypeID& cUT) :
  internalName(iN),
  dynamicData(dD),
  energyCost(eC),
  metalCost(mC),
  fixed(f),
  ground(gro),
  surface(su),
  gravity(gra),
  seabed(se),
  weapons(w),
  corpseUnitType(cUT)
{
}

UnitType::UnitType(
    String iN,
    const UnitTypeData& dD,
    uint32 eC,
    uint32 mC,
    bool f,
    bool gro,
    bool su,
    bool gra,
    bool se,
    const std::list<String>& w,
    const String& cUT) :
  internalName(iN),
  dynamicData(dD),
  energyCost(eC),
  metalCost(mC),
  fixed(f),
  ground(gro),
  surface(su),
  gravity(gra),
  seabed(se),
  weaponNames(w),
  corpseUnitTypeName(cUT),
  corpseUnitType(NULL)
{
}

String UnitType::resolveNames(const Universe* universe)
{
  assert(weapons.empty());
  assert(corpseUnitType == NULL);
  
  while (!weaponNames.empty()) {
    WeaponTypeID weaponID = universe->getWeaponTypeID(weaponNames.front());
    if (weaponID == NULL) {
      return weaponNames.front();
    }
    weaponNames.pop_front();
    weapons.push_back(weaponID);
  }

  if (corpseUnitTypeName != "") {
    UnitTypeID corpseID = universe->getUnitTypeId(corpseUnitTypeName);
    if (corpseID == NULL) {
      return corpseUnitTypeName;
    }
    corpseUnitTypeName = "";
    corpseUnitType = corpseID;
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
  archive << fixed << ground << surface << gravity << seabed << weapons <<
    (corpseUnitType == NULL ? String("") : corpseUnitType->getInternalName());
    /* FIXME: assumes UnitTypeID is UnitType* */
}

UnitType UnitType::load(IArchive& archive)
{
  String internalName;
  archive >> internalName;
  UnitTypeData dynamicData = UnitTypeData::load(archive);
  uint32 energyCost;
  uint32 metalCost;
  archive >> energyCost >> metalCost;
  bool fixed;
  bool ground;
  bool surface;
  bool gravity;
  bool seabed;
  std::list<String> weapons;
  String corpseUnitType;
  archive >> fixed >> ground >> surface >> gravity >> seabed >> weapons >>
    corpseUnitType;

  return UnitType(
      internalName, dynamicData, energyCost, metalCost, fixed,
      ground, surface, gravity, seabed, weapons, corpseUnitType
    );
}

