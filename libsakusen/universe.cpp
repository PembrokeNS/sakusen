#include "universe.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

Universe::Universe(
    const String& in,
    const String& h,
    const std::vector<WeaponType>& w,
    const std::vector<UnitType>& u
  ) :
  internalName(in),
  hash(h),
  weaponTypes(w),
  unitTypes(u)
{
  constructHashMaps();
}

void Universe::constructHashMaps()
{
  for (vector<WeaponType>::iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    if (weaponIDLookup.count(weaponType->getInternalName())) {
      throw DuplicateNameDeserializationExn(weaponType->getInternalName());
    }
    weaponIDLookup[weaponType->getInternalName()] = &*weaponType;
  }
  
  for (vector<UnitType>::iterator unitType = unitTypes.begin();
      unitType != unitTypes.end(); unitType++) {
    if (unitIDLookup.count(unitType->getInternalName())) {
      throw DuplicateNameDeserializationExn(unitType->getInternalName());
    }
    unitIDLookup[unitType->getInternalName()] = &*unitType;
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

WeaponTypeID Universe::getWeaponTypeID(String weaponTypeName) const
{
  hash_map_string<WeaponTypeID>::type::const_iterator
    weaponType = weaponIDLookup.find(weaponTypeName);
  if (weaponType == weaponIDLookup.end()) {
    return NULL;
  }
  return weaponType->second;
}

UnitTypeID Universe::getUnitTypeId(String unitTypeName) const
{
  hash_map_string<UnitTypeID>::type::const_iterator
    unitType = unitIDLookup.find(unitTypeName);
  if (unitType == unitIDLookup.end()) {
    Debug("unit type '"+unitTypeName+"' not found");
    return NULL;
  }
  return unitType->second;
}

bool Universe::containsUnitType(const UnitTypeID id) const
{
  /** \todo Rewrite using unitIDLookup so that it takes only O(1) time */
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
  archive << internalName << weaponTypes << unitTypes;
}

Universe* Universe::loadNew(
    IArchive& archive,
    const DeserializationContext& context
  )
{
  String internalName;
  vector<WeaponType> weaponTypes;
  vector<UnitType> unitTypes;
  (archive >> internalName).extract(weaponTypes, context) >>
    unitTypes;
  Universe* u = new Universe(
      internalName, archive.getSecureHashAsString(), weaponTypes, unitTypes
    );
  String unresolvedName;
  if ("" != (unresolvedName = u->resolveNames())) {
    delete u;
    throw UnresolvedNameDeserializationExn(unresolvedName);
  }
  return u;
}

