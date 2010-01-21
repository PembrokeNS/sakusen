#include "universe.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

Universe::Universe(
    const String& in,
    const String& h,
    const String& sM,
    const String& sF,
    const String& pF,
    const std::vector<WeaponType>& w,
    const std::vector<UnitType>& u,
    const uint32 lMinSNS,
    const uint32 lMaxSNS
  ) :
  internalName(in),
  hash(h),
  scriptModule(sM),
  scriptFunction(sF),
  playerDataFunction(pF),
  weaponTypes(w),
  unitTypes(u),
  logMinSpatialNoiseScale(lMinSNS),
  logMaxSpatialNoiseScale(lMaxSNS)
{
  constructHashMaps();
}

void Universe::constructHashMaps()
{
  for (vector<WeaponType>::iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    if (weaponIdLookup.count(weaponType->getInternalName())) {
      throw DuplicateNameDeserializationExn(weaponType->getInternalName());
    }
    weaponIdLookup[weaponType->getInternalName()] = &*weaponType;
  }
  
  for (vector<UnitType>::iterator unitType = unitTypes.begin();
      unitType != unitTypes.end(); unitType++) {
    if (unitIdLookup.count(unitType->getInternalName())) {
      throw DuplicateNameDeserializationExn(unitType->getInternalName());
    }
    unitIdLookup[unitType->getInternalName()] = &*unitType;
  }
}

String Universe::resolveNames()
{
  for (vector<UnitType>::iterator unitType = unitTypes.begin();
      unitType != unitTypes.end(); unitType++) {
    String name;
    if ("" != (name = unitType->resolveNames(this))) {
      return name;
    }
  }

  return "";
}

bool Universe::sanityCheck() const
{
  /** \todo Check something */
  return true;
}

WeaponTypeId Universe::getWeaponTypeId(String weaponTypeName) const
{
  u_map<String, WeaponTypeId>::type::const_iterator
    weaponType = weaponIdLookup.find(weaponTypeName);
  if (weaponType == weaponIdLookup.end()) {
    return NULL;
  }
  return weaponType->second;
}

UnitTypeId Universe::getUnitTypeId(String unitTypeName) const
{
  u_map<String, UnitTypeId>::type::const_iterator
    unitType = unitIdLookup.find(unitTypeName);
  if (unitType == unitIdLookup.end()) {
    Debug("unit type '"+unitTypeName+"' not found");
    return NULL;
  }
  return unitType->second;
}

bool Universe::containsUnitType(const UnitTypeId id) const
{
  /** \todo Rewrite using unitIdLookup so that it takes only O(1) time */
  const UnitType* typePtr = getUnitTypePtr(id);
  for (std::vector<UnitType>::const_iterator unitType = unitTypes.begin();
      unitType != unitTypes.end(); unitType++) {
    if (typePtr == &*unitType) {
      return true;
    }
  }
  return false;
}

void Universe::store(OArchive& archive) const
{
  archive << internalName << scriptModule << scriptFunction <<
    playerDataFunction<< logMinSpatialNoiseScale << logMaxSpatialNoiseScale <<
    weaponTypes << unitTypes;
}

Universe* Universe::loadNew(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  String internalName;
  String scriptModule;
  String scriptFunction;
  String playerDataFunction;
  vector<WeaponType> weaponTypes;
  vector<UnitType> unitTypes;
  uint32 logMinSpatialNoiseScale;
  uint32 logMaxSpatialNoiseScale;
  archive >> internalName >> scriptModule >> scriptFunction >>
    playerDataFunction >> logMinSpatialNoiseScale >> logMaxSpatialNoiseScale;
  archive.extract(weaponTypes, context) >> unitTypes;
  Universe* u = new Universe(
      internalName, archive.getSecureHashAsString(),
      scriptModule, scriptFunction, playerDataFunction, weaponTypes, unitTypes,
      logMinSpatialNoiseScale, logMaxSpatialNoiseScale
    );
  String unresolvedName;
  if ("" != (unresolvedName = u->resolveNames())) {
    delete u;
    throw UnresolvedNameDeserializationExn(unresolvedName);
  }
  return u;
}

