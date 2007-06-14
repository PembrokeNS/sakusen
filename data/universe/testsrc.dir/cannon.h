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

using namespace sakusen;
using namespace sakusen::server;

namespace testsrc {

class BuildingLayer : public UnitMask {
  public:
    typedef boost::shared_ptr<BuildingLayer> Ptr;
    typedef boost::shared_ptr<const BuildingLayer> ConstPtr;
    typedef boost::weak_ptr<BuildingLayer> WPtr;
    typedef boost::weak_ptr<const BuildingLayer> WConstPtr;

    BuildingLayer() :
      UnitMask(),
      builtHitPoints(1),
      empty(new SphereRegion<sint16>(Point<sint16>(), 0)),
      blind()
    {
    }
  private:
    BuildingLayer(const BuildingLayer& copy, LayeredUnit* outer) :
      UnitMask(copy, outer),
      builtHitPoints(copy.builtHitPoints),
      empty(copy.empty),
      blind(copy.blind)
    {}

    HitPoints builtHitPoints;
    const Region<sint16>::ConstPtr empty;
    const Sensors blind;
  public:
    UnitLayer::Ptr newCopy(LayeredUnit* outer) const {
      return UnitLayer::Ptr(new BuildingLayer(*this, outer));
    }

    HitPoints getMaxHitPoints() const {
      return builtHitPoints;
    }

    /* Prevent constructing thing from moving */
    Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return empty;
    }
    Region<sint16>::ConstPtr getPossibleVelocities() const {
      return empty;
    }
    Region<sint16>::ConstPtr getPossibleAngularVelocities() const {
      return empty;
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
    Weapon* newCopy() const { return new FactoryCreater(*this); }
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
    Weapon* newCopy() const { return new GruntCreater(*this); }
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
    Weapon* newCopy() const { return new SpiderCreater(*this); }
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
    Weapon* newCopy() const { return new Builder(*this); }
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
    Weapon* newCopy() const {
      return new Cannon(*this);
    }
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
    Weapon* newCopy() const { return new Paralyzer(*this); }
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
    void onInteractUnit(double position, const Ref<LayeredUnit>&);
};

class ParalyzingLayer : public UnitMask, public Fuse {
  friend class ParalyzationBeam;
  public:
    typedef boost::shared_ptr<ParalyzingLayer> Ptr;
    typedef boost::shared_ptr<const ParalyzingLayer> ConstPtr;

    ParalyzingLayer() :
      empty(new SphereRegion<sint16>(Point<sint16>(), 0)),
      blind(),
      token(-1)
    {}
  private:
    ParalyzingLayer(const ParalyzingLayer& copy, LayeredUnit* outer) :
      UnitMask(copy, outer),
      empty(copy.empty),
      blind(copy.blind)
    {}
    const Region<sint16>::ConstPtr empty;
    const Sensors blind;
    FuseToken token;
  public:
    UnitLayer::Ptr newCopy(LayeredUnit* outer) const {
      Fatal("Can't copy because also fuse.");
      return UnitLayer::Ptr(new ParalyzingLayer(*this, outer));
    }
    /* Prevent paralyzed thing from moving */
    Region<sint16>::ConstPtr getPossibleAccelerations() const {
      return empty;
    }
    Region<sint16>::ConstPtr getPossibleVelocities() const {
      return empty;
    }
    Region<sint16>::ConstPtr getPossibleAngularVelocities() const {
      return empty;
    }
    const Sensors& getVision() const {
      return blind;
    }

    void incrementWeaponsState() {
      /* Weapons do nothing when unit under construction */
    }

    /* Remove self whe fuse expires */
    void expire(FuseToken);
    void onRemoval();
};

}

#endif // CANNON_H

