#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"
#include "ballistic.h"
#include "layeredunit.h"
#include "effect.h"
#include "sphereregion.h"
#include "beam.h"
#include "unitmask.h"
#include "fuse.h"
#include "script.h"
#include "playerdata.h"

#if defined(_MSC_VER)
/* The __declspec stuff for ensuring symbols are exported from DLLs and
 * imported back into libraries.
 *
 * This being a module opened at runtime, we always export and never import. */
  #define MODULE_API   __declspec(dllexport)
  #define MODULE_EXIMP extern
#else
  #if defined(__GNUC__) && (__GNUC__ >= 4)
    #define MODULE_API __attribute__ ((visibility ("default")))
  #else
    #define MODULE_API
  #endif
#endif

using namespace sakusen;
using namespace sakusen::server;

namespace testsrc {

class TestScript : public Script {
  public:
    void unitAdded(const Ref<LayeredUnit>&);
};

class TestPlayerData : public PlayerData {
};

class MaterielCreater : public UnitMask {
  public:
    MaterielCreater() :
      UnitMask(),
      energyPerTick(10),
      metalPerTick(2)
    {}
  private:
    uint32 energyPerTick;
    uint32 metalPerTick;
  public:
    void incrementState();
};

class BuildingLayer : public UnitMask {
  public:
    typedef boost::shared_ptr<BuildingLayer> Ptr;
    typedef boost::shared_ptr<const BuildingLayer> ConstPtr;
    typedef boost::weak_ptr<BuildingLayer> WPtr;
    typedef boost::weak_ptr<const BuildingLayer> WConstPtr;

    BuildingLayer() :
      UnitMask(),
      builtHitPoints(1),
      empty16(new SphereRegion<sint16>(Point<sint16>(), 0)),
      empty32(new SphereRegion<sint32>(Point<sint32>(), 0)),
      blind()
    {
    }
  private:
    HitPoints builtHitPoints;
    const Region<sint16>::ConstPtr empty16;
    const Region<sint32>::ConstPtr empty32;
    const Sensors blind;
  public:
    HitPoints getMaxHitPoints() const {
      return builtHitPoints;
    }

    /* Prevent constructing thing from moving */
    Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return empty16;
    }
    Region<sint16>::ConstPtr getPossibleVelocities() const {
      return empty16;
    }
    Region<sint32>::ConstPtr getPossibleAngularVelocities() const {
      return empty32;
    }
    const Sensors& getVision() const {
      return blind;
    }

    void incrementWeaponsState() {
      /* Weapons do nothing when unit under construction */
    }

    HitPoints build(HitPoints amount);
};

class Creater : public Weapon {
  public:
    Creater(const WeaponType* type) :
      Weapon(type)
    {}
  private:
    bool aim(
        const Ref<LayeredUnit>& source,
        WeaponStatus& status,
        const WeaponOrders&
      );
    /* This method is irrelevant for builders */
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(
        const Ref<LayeredUnit>& firer,
        const WeaponStatus&,
        WeaponOrders&,
        uint16 weaponIndex
      );
  protected:
    /* When overriden, gives the maximum range at which creation can take place
     * */
    virtual uint64 squareRange() const = 0;
    /* When overriden, gives the UnitType created */
    virtual UnitTypeId getTypeCreated() const = 0;
};

class FactoryCreater : public Creater {
  public:
    FactoryCreater(const WeaponType* type) :
      Creater(type)
    {}
  private:
    uint64 squareRange() const { return 100000000; }
    UnitTypeId getTypeCreated() const {
      return world->getUniverse()->getUnitTypeId("factory");
    }
};

class GruntCreater : public Creater {
  public:
    GruntCreater(const WeaponType* type) :
      Creater(type)
    {}
  private:
    uint64 squareRange() const { return 30000000; }
    UnitTypeId getTypeCreated() const {
      return world->getUniverse()->getUnitTypeId("grunt");
    }
};

class SpiderCreater : public Creater {
  public:
    SpiderCreater(const WeaponType* type) :
      Creater(type)
    {}
  private:
    uint64 squareRange() const { return 30000000; }
    UnitTypeId getTypeCreated() const {
      return world->getUniverse()->getUnitTypeId("spider");
    }
};

class Builder : public Weapon {
  public:
    Builder(const WeaponType* type) :
      Weapon(type)
    {}
  private:
    BuildingLayer::WPtr buildingLayer;
    bool aim(
        const Ref<LayeredUnit>& source,
        WeaponStatus&,
        const WeaponOrders&
      );
    /* This method is irrelevant for builders */
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(
        const Ref<LayeredUnit>& firer,
        const WeaponStatus&,
        WeaponOrders&,
        uint16 weaponIndex
      );
  protected:
    /* Gives the maximum range at which building can take place
     * */
    virtual uint64 squareRange() const { return 100000000; }
};

class Cannon : public Weapon {
  public:
    Cannon(const WeaponType* type) :
      Weapon(type)
    {}
  protected:
    uint32 getProjectileSpeed() const { return 2000; }
    void onFire(
        const Ref<LayeredUnit>& firer,
        const WeaponStatus&,
        WeaponOrders&,
        uint16 weaponIndex
      );
  public:
};

class Shell : public Ballistic {
  public:
    Shell(const Ref<LayeredUnit>& source, const WeaponStatus& status);

    void onCollision(const Point<sint32>& pos);
};

class Explosion : public Effect {
  public:
    Explosion(PlayerId owner, const Point<sint32>& centre, uint32 radius) :
      Effect(
          owner, Region<sint32>::Ptr(new SphereRegion<sint32>(centre, radius)),
          Visibility()
        ) {}
    void onUnitPresent(const Ref<LayeredUnit>&);
};

class Paralyzer : public Weapon {
  public:
    Paralyzer(const WeaponType* type) : Weapon(type) {}
  protected:
    bool aim(
        const Ref<LayeredUnit>& firer,
        WeaponStatus&,
        const WeaponOrders&
      );
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(
        const Ref<LayeredUnit>& firer,
        const WeaponStatus&,
        WeaponOrders&,
        uint16 weaponIndex
      );
  public:
};

class ParalyzationBeam : public Beam {
  public:
    ParalyzationBeam(
        const Ref<LayeredUnit>& source,
        const WeaponStatus& status
      );
    GameObject interactsWith() const {
      return gameObject_unit | gameObject_land;
    }
    GameObject stoppedBy() const {
      return gameObject_unit | gameObject_land;
    }
    void onInteractLand(double position);
    void onInteractUnit(double position, const Ref<LayeredUnit>&, bool leaving);
};

class ParalyzingLayer : public UnitMask, public Fuse {
  friend class ParalyzationBeam;
  public:
    typedef boost::shared_ptr<ParalyzingLayer> Ptr;
    typedef boost::shared_ptr<const ParalyzingLayer> ConstPtr;

    ParalyzingLayer() :
      empty16(new SphereRegion<sint16>(Point<sint16>(), 0)),
      empty32(new SphereRegion<sint32>(Point<sint32>(), 0)),
      blind(),
      token(FuseToken(-1))
    {}
  private:
    const Region<sint16>::ConstPtr empty16;
    const Region<sint32>::ConstPtr empty32;
    const Sensors blind;
    FuseToken token;
  public:
    /* Prevent paralyzed thing from moving */
    Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return empty16;
    }
    Region<sint16>::ConstPtr getPossibleVelocities() const {
      return empty16;
    }
    Region<sint32>::ConstPtr getPossibleAngularVelocities() const {
      return empty32;
    }
    const Sensors& getVision() const {
      return blind;
    }

    void incrementWeaponsState() {
      /* Weapons do nothing when unit paralyzed */
    }

    /* Remove self whe fuse expires */
    void expire(FuseToken);
    void onRemoval();
};

}

#endif // CANNON_H

