#ifndef UNITTYPE_H
#define UNITTYPE_H

#include "libsakusen-global.h"
#include "point.h"
#include "visibility.h"
#include "weapon.h"
#include "iarchive.h"
#include "oarchive.h"
#include "hitpoints.h"

#include <list>

class Universe;
class UnitType;

typedef const UnitType* UnitTypeID;

class LIBSAKUSEN_API UnitType {
  private:
    UnitType();
  public:
    /** Constructs from all required data */
    UnitType(
      String internalName,
      HitPoints maxHitPoints,
      uint32 energyCost,
      uint32 metalCost,
      uint8 mass,
      Point<uint32> size,
      Point<uint16> maxAcceleration,
      Point<uint16> maxMinusAcceleration,
      Point<sint16> maxSpeed,
      Point<sint16> maxMinusSpeed,
      Angle maxHorizTurnSpeed,
      Angle maxVertTurnSpeed,
      const Visibility& visibility,
      const Sensors& vision,
      bool fixed,
      bool ground,
      bool surface,
      bool air,
      bool seabed,
      const std::list<WeaponTypeID>& weapons,
      const UnitTypeID& corpseUnitType
    );
    /** \brief Constructs from most required data, with some details postponed
     *
     * This constructor is almost the same as the above constructor, but rather
     * than explicit WeaponTypeIDs and UnitTypeIDs, it takes Strings which will
     * be resolved into IDs later.
     *
     * \warning After calling this constructor, and before
     * using this UnitType, you *must* call UnitType::resolveNames to finish
     * the initilaization.
     */
    UnitType(
      String internalName,
      HitPoints maxHitPoints,
      uint32 energyCost,
      uint32 metalCost,
      uint8 mass,
      Point<uint32> size,
      Point<uint16> maxAcceleration,
      Point<uint16> maxMinusAcceleration,
      Point<sint16> maxSpeed,
      Point<sint16> maxMinusSpeed,
      Angle maxHorizTurnSpeed,
      Angle maxVertTurnSpeed,
      const Visibility& visibility,
      const Sensors& vision,
      bool fixed,
      bool ground,
      bool surface,
      bool air,
      bool seabed,
      const std::list<String>& weapons,
      const String& corpseUnitType
    );
    virtual ~UnitType() {}
  private:
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
    Visibility visibility;
    Sensors vision;
    bool fixed:1; /* true for buildings, mines */
    bool ground:1; /* true for buildings, ground units, planes that can land
                      without a runway, hovercraft */
    bool surface:1; /* true for ships and floating buildings */
    bool air:1; /* true for planes, missiles */
    bool seabed:1; /* true for underwater metal mines, tanks that can drive on
                      the seabed */
    std::list<String> weaponNames;
    std::list<WeaponTypeID> weapons;
    String corpseUnitTypeName;
    UnitTypeID corpseUnitType;
  public:
    PLACEHOLDER_OPERATORS(UnitType);

    String resolveNames(const Universe* universe);

    /* accessors */
    inline const String& getInternalName() const { return internalName; }
    inline const HitPoints& getMaxHitPoints() const { return maxHitPoints; }
    inline const uint32& getEnergyCost() const { return energyCost; }
    inline const uint32& getMetalCost() const { return metalCost; }
    inline const uint8& getMass() const { return mass; }
    inline const Point<uint32>& getSize() const { return size; }
    inline const Point<uint16>& getMaxAcceleration() const { return maxAcceleration; }
    inline const Point<uint16>& getMaxMinusAcceleration() const { return maxMinusAcceleration; }
    inline const Point<sint16>& getMaxSpeed() const { return maxSpeed; }
    inline const Point<sint16>& getMaxMinusSpeed() const { return maxMinusSpeed; }
    inline const Angle& getMaxYawSpeed() const { return maxYawSpeed; }
    inline const Angle& getMaxPitchSpeed() const { return maxPitchSpeed; }
    inline const Visibility& getVisibility() const { return visibility; }
    inline const Sensors& getVision() const { return vision; }
    inline const std::list<WeaponTypeID> getWeapons() const { return weapons; }
    inline const UnitTypeID& getCorpseUnitType() const { return corpseUnitType; }
    
    void store(OArchive&) const;
    static UnitType load(IArchive&);
};

#ifdef NEED_TEMPLATE_INSTANTIATION
#include <vector>
#pragma warning (disable: 4231)
EXPORT_VECTOR(UnitType)
#endif

#endif
