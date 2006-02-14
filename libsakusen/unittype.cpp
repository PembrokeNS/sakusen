#include "unittype.h"

#include "universe.h"
#include "region-methods.h"
#include "oarchive-methods.h"

using namespace sakusen;

UnitType::UnitType(
    String in,
    HitPoints mHP,
    uint32 eC,
    uint32 mC,
    uint8 m,
    Point<uint32> si,
    Region<sint16> pA,
    Region<sint16> pV,
    Region<sint16> pAV,
    const Visibility& visib,
    const Sensors& visio,
    bool f,
    bool g,
    bool su,
    bool a,
    bool se,
    const std::list<WeaponTypeID>& w,
    const UnitTypeID& cUT) :
  internalName(in),
  maxHitPoints(mHP),
  energyCost(eC),
  metalCost(mC),
  mass(m),
  size(si),
  possibleAccelerations(pA),
  possibleVelocities(pV),
  possibleAngularVelocities(pAV),
  visibility(visib),
  vision(visio),
  fixed(f),
  ground(g),
  surface(su),
  air(a),
  seabed(se),
  weapons(w),
  corpseUnitType(cUT)
{
}

UnitType::UnitType(
    String in,
    HitPoints mHP,
    uint32 eC,
    uint32 mC,
    uint8 m,
    Point<uint32> si,
    Region<sint16> pA,
    Region<sint16> pV,
    Region<sint16> pAV,
    const Visibility& visib,
    const Sensors& visio,
    bool f,
    bool g,
    bool su,
    bool a,
    bool se,
    const std::list<String>& w,
    const String& cUT) :
  internalName(in),
  maxHitPoints(mHP),
  energyCost(eC),
  metalCost(mC),
  mass(m),
  size(si),
  possibleAccelerations(pA),
  possibleVelocities(pV),
  possibleAngularVelocities(pAV),
  visibility(visib),
  vision(visio),
  fixed(f),
  ground(g),
  surface(su),
  air(a),
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
    UnitTypeID corpseID = universe->getUnitTypeID(corpseUnitTypeName);
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
  archive << internalName << maxHitPoints <<
    energyCost << metalCost << mass << size;
  possibleAccelerations.store(archive);
  possibleVelocities.store(archive);
  possibleAngularVelocities.store(archive);
  visibility.store(archive);
  vision.store(archive);
  archive << fixed << ground << surface << air << seabed << weapons <<
    (corpseUnitType == NULL ? String("") : corpseUnitType->getInternalName());
    /* FIXME: assumes UnitTypeID is UnitType* */
}

UnitType UnitType::load(IArchive& archive)
{
  String internalName;
  HitPoints maxHitPoints;
  uint32 energyCost;
  uint32 metalCost;
  /* armour */
  uint8 mass;
  Point<uint32> size;
  archive >> internalName >> maxHitPoints >>
    energyCost >> metalCost >> mass >> size;
  Region<sint16> possibleAccelerations = Region<sint16>::load(archive);
  Region<sint16> possibleVelocities = Region<sint16>::load(archive);
  Region<sint16> possibleAngularVelocities = Region<sint16>::load(archive);
  Visibility visibility(archive);
  Sensors vision(archive);
  bool fixed;
  bool ground;
  bool surface;
  bool air;
  bool seabed;
  std::list<String> weapons;
  String corpseUnitType;
  archive >> fixed >> ground >> surface >> air >> seabed >> weapons >>
    corpseUnitType;

  return UnitType(
      internalName,
      maxHitPoints, energyCost, metalCost, mass, size, possibleAccelerations,
      possibleVelocities, possibleAngularVelocities, visibility, vision, fixed,
      ground, surface, air, seabed, weapons, corpseUnitType
    );
}

