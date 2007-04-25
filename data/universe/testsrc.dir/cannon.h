#ifndef CANNON_H
#define CANNON_H

#include "weapon.h"
#include "ballistic.h"
#include "layeredunit.h"
#include "effect.h"
#include "sphereregion.h"
#include "beam.h"
#include "unitmask.h"

using namespace sakusen;
using namespace sakusen::server;

namespace testsrc {

class BuildingLayer : public UnitMask {
  public:
    typedef boost::shared_ptr<BuildingLayer> Ptr;
    typedef boost::shared_ptr<const BuildingLayer> ConstPtr;
    typedef boost::weak_ptr<BuildingLayer> WPtr;
    typedef boost::weak_ptr<const BuildingLayer> WConstPtr;

    BuildingLayer() : UnitMask() {}
  private:
    BuildingLayer(const BuildingLayer& copy, LayeredUnit* outer) :
      UnitMask(copy, outer)
    {}
  public:
    UnitLayer::Ptr newCopy(LayeredUnit* outer) const {
      return UnitLayer::Ptr(new BuildingLayer(*this, outer));
    }

    void build();
};

class Creater : public Weapon {
  public:
    Creater(const WeaponType* type) :
      Weapon(type)
    {}
  private:
    bool aim(
        const Ref<LayeredUnit>& source,
        WeaponStatus* status,
        const WeaponOrders&
      );
    /* This method is irrelevant for builders */
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex);
  protected:
    /* When overriden, gives the maximum range at which creation can take place
     * */
    virtual uint64 squareRange() const = 0;
    /* When overriden, gives the UnitType created */
    virtual UnitTypeID getTypeCreated() const = 0;
};

class FactoryCreater : public Creater {
  public:
    FactoryCreater(const WeaponType* type) :
      Creater(type)
    {}
  private:
    Weapon* newCopy() const { return new FactoryCreater(*this); }
    uint64 squareRange() const { return 3000000; }
    UnitTypeID getTypeCreated() const {
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
    uint64 squareRange() const { return 3000000; }
    UnitTypeID getTypeCreated() const {
      return world->getUniverse()->getUnitTypeId("grunt");
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
        WeaponStatus* status,
        const WeaponOrders&
      );
    /* This method is irrelevant for builders */
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex);
    Weapon* newCopy() const { return new Builder(*this); }
  protected:
    /* Gives the maximum range at which building can take place
     * */
    virtual uint64 squareRange() const { return 3000000; }
};

class Cannon : public Weapon {
  public:
    Cannon(const WeaponType* type) :
      Weapon(type)
    {}
  protected:
    uint32 getProjectileSpeed() const { return 2000; }
    void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex);
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
    Explosion(PlayerID owner, const Point<sint32>& centre, uint32 radius) :
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
        WeaponStatus* status,
        const WeaponOrders&
      );
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex);
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

}

#endif // CANNON_H

