#include "unittype.h"

#include "universe.h"
#include "iarchive-methods.h"
#include "oarchive-methods.h"

UnitType::UnitType(
    String in,
    HitPoints mHP,
    uint32 eC,
    uint32 mC,
    uint8 m,
    Point<uint32> si,
    Point<uint16> mA,
    Point<uint16> mMA,
    Point<sint16> mS,
    Point<sint16> mMS,
    Angle mHTS,
    Angle mVTS,
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
  maxAcceleration(mA),
  maxMinusAcceleration(mMA),
  maxSpeed(mS),
  maxMinusSpeed(mMS),
  maxYawSpeed(mHTS),
  maxPitchSpeed(mVTS),
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
    Point<uint16> mA,
    Point<uint16> mMA,
    Point<sint16> mS,
    Point<sint16> mMS,
    Angle mHTS,
    Angle mVTS,
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
  maxAcceleration(mA),
  maxMinusAcceleration(mMA),
  maxSpeed(mS),
  maxMinusSpeed(mMS),
  maxYawSpeed(mHTS),
  maxPitchSpeed(mVTS),
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
    energyCost << metalCost << mass << size <<
    maxAcceleration << maxMinusAcceleration << maxSpeed << maxMinusSpeed <<
    maxYawSpeed << maxPitchSpeed;
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
  Point<uint16> maxAcceleration;
  Point<uint16> maxMinusAcceleration;
  Point<sint16> maxSpeed;
  Point<sint16> maxMinusSpeed;
  Angle maxYawSpeed;
  Angle maxPitchSpeed;
  archive >> internalName >> maxHitPoints >>
    energyCost >> metalCost >> mass >> size >>
    maxAcceleration >> maxMinusAcceleration >> maxSpeed >> maxMinusSpeed >>
    maxYawSpeed >> maxPitchSpeed;
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
      maxHitPoints, energyCost, metalCost, mass, size, maxAcceleration,
      maxMinusAcceleration, maxSpeed, maxMinusSpeed, maxYawSpeed,
      maxPitchSpeed, visibility, vision, fixed, ground, surface, air, seabed,
      weapons, corpseUnitType
    );
}

