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

    void build();
};

class Builder : public Weapon {
  public:
    Builder(const WeaponType* type) :
      Weapon(type)
    {}
  private:
    BuildingLayer::WPtr buildingLayer;
    /* These types of aiming not supported */
    bool aimAt(
        const Ref<LayeredUnit>&,
        WeaponStatus*,
        const Position&,
        const Velocity&
      ) {
      return false;
    }
    bool aimAt(
        const Ref<LayeredUnit>& source,
        WeaponStatus* status,
        const Position&,
        const Orientation&
      );
    bool aimAt(
        const Ref<LayeredUnit>& source,
        WeaponStatus* status,
        const Ref<LayeredUnit>& target
      );
    /* This method is irrelevant for builders */
    uint32 getProjectileSpeed() const { return 0; }
    void onFire(const Ref<LayeredUnit>& firer, uint16 weaponIndex);
  protected:
    /* When overriden, gives the maximum range at which building can take place
     * */
    virtual uint64 squareRange() const = 0;
};

class FactoryBuilder : public Builder {
  public:
    FactoryBuilder(const WeaponType* type) :
      Builder(type)
    {}
  private:
    Weapon* newCopy() const { return new FactoryBuilder(*this); }
    uint64 squareRange() const { return 10000; }
};

class GruntBuilder : public Builder {
  public:
    GruntBuilder(const WeaponType* type) :
      Builder(type)
    {}
  private:
    Weapon* newCopy() const { return new GruntBuilder(*this); }
    uint64 squareRange() const { return 10000; }
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
    bool aimAt(
        const Ref<LayeredUnit>& firer,
        WeaponStatus* status,
        const Point<sint32>& pos,
        const Point<sint16>& vel
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

