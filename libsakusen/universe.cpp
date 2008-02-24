#include "universe.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

Universe::Universe(
    const String& in,
    const String& h,
    const String& sM,
    const String& sF,
    const std::vector<WeaponType>& w,
    const std::vector<UnitType>& u
  ) :
  internalName(in),
  hash(h),
  scriptModule(sM),
  scriptFunction(sF),
  weaponTypes(w),
  unitTypes(u)
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

WeaponTypeId Universe::getWeaponTypeId(String weaponTypeName) const
{
  hash_map_string<WeaponTypeId>::type::const_iterator
    weaponType = weaponIdLookup.find(weaponTypeName);
  if (weaponType == weaponIdLookup.end()) {
    return NULL;
  }
  return weaponType->second;
}

UnitTypeId Universe::getUnitTypeId(String unitTypeName) const
{
  hash_map_string<UnitTypeId>::type::const_iterator
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
  archive << internalName << scriptModule << scriptFunction << weaponTypes <<
    unitTypes;
}

Universe* Universe::loadNew(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  String internalName;
  String scriptModule;
  String scriptFunction;
  vector<WeaponType> weaponTypes;
  vector<UnitType> unitTypes;
  archive >> internalName >> scriptModule >> scriptFunction;
  archive.extract(weaponTypes, context) >> unitTypes;
  Universe* u = new Universe(
      internalName, archive.getSecureHashAsString(),
      scriptModule, scriptFunction, weaponTypes, unitTypes
    );
  String unresolvedName;
  if ("" != (unresolvedName = u->resolveNames())) {
    delete u;
    throw UnresolvedNameDeserializationExn(unresolvedName);
  }
  return u;
}

