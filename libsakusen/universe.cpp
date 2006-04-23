#include "universe.h"

#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

Universe::Universe(
    const String& in,
    const String& h,
    const std::vector<UnitType>& u
  ) :
  internalName(in),
  hash(h),
  unitTypes(u)
{
  constructHashMaps();
}

void Universe::constructHashMaps()
{
  for (vector<UnitType>::iterator unitType = unitTypes.begin();
      unitType != unitTypes.end(); unitType++) {
    if (unitIDLookup.count(unitType->getInternalName())) {
      throw new
        DuplicateNameDeserializationExn(unitType->getInternalName());
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

WeaponTypeID Universe::getWeaponTypeID(String /*weaponTypeName*/) const
{
  /* TODO: fill this in when Universe actually has a store of WeaponTypes */
  return NULL;
}

UnitTypeID Universe::getUnitTypeID(String unitTypeName) const
{
  if (0 == unitIDLookup.count(unitTypeName)) {
    return NULL;
  }
  return unitIDLookup.find(unitTypeName)->second;
}

bool Universe::containsUnitType(const UnitTypeID id) const
{
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
  archive << internalName << unitTypes;
}

Universe* Universe::loadNew(IArchive& archive)
{
  String internalName;
  vector<UnitType> unitTypes;
  archive >> internalName >> unitTypes;
  Universe* u =
    new Universe(internalName, archive.getSecureHashAsString(), unitTypes);
  String unresolvedName;
  if ("" != (unresolvedName = u->resolveNames())) {
    delete u;
    throw new UnresolvedNameDeserializationExn(unresolvedName);
  }
  return u;
}

