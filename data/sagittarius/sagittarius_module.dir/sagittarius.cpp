#include <boost/typeof/typeof.hpp>

#include "weapontype.h"
#include "ref.h"
#include "layeredunit.h"
#include "weapon.h"
#include "beam.h"
#include "completeworld.h"
#include "sphereregion.h"
#include "unitmask.h"
#include "ispatial.h"

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

#define CM 100000

using namespace std;
using namespace sakusen;
using namespace sakusen::server;

class Laserator: public Weapon {
	public:
		Laserator(const WeaponType* type) : Weapon(type) {}
		bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
		void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
		uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
	};

class LaserBeam: public Beam {
	public:
		LaserBeam(const Ref<LayeredUnit>& source, const WeaponStatus& status);
		GameObject interactsWith() const { return gameObject_unit; }
		GameObject stoppedBy() const { return gameObject_unit; }
		void onInteractUnit(double position, const Ref<LayeredUnit>&, bool leaving);
	};

class SagScript: public Script {
	};

class SagPlayerData: public PlayerData {
	public:
		SagPlayerData() : attack(0), defense(0), speed(0) {};
		uint16 attack;
		uint16 defense;
		uint16 speed;
	};

class TorpedoLauncher: public Weapon {
	public:
		TorpedoLauncher(const WeaponType* type) : Weapon(type), lastFire(0) {}
		bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
		void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
		uint32 getProjectileSpeed() const { return 30000; } //Unused as this is non-ballisticm
		Time lastFire;
	};

class AttackSetter: public Weapon {
        public:
                AttackSetter(const WeaponType* type) : Weapon(type) {}
                bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
                void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
                uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
        };

class DefenseSetter: public Weapon {
        public:
                DefenseSetter(const WeaponType* type) : Weapon(type) {}
                bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
                void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
                uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
        };

class SpeedSetter: public Weapon {
        public:
                SpeedSetter(const WeaponType* type) : Weapon(type) {}
                bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
                void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
                uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
        };

class FleetCreator: public Weapon {
        public:
                FleetCreator(const WeaponType* type) : Weapon(type) {}
                bool aim(const Ref<LayeredUnit>& firer, WeaponStatus&, const WeaponOrders& orders);
                void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders&, uint16 weaponIndex);
                uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
        };

class TorpedoDetonator: public UnitMask {
	public:
		void incrementState();
	};

class TorpedoFuse: public UnitMask, public Fuse {
	public:
		void expire(FuseToken);
	};

bool Laserator::aim(const Ref<LayeredUnit>& firer, WeaponStatus& status, const WeaponOrders& orders) {
	BOOST_AUTO(displacement ,orders.getTargetPosition() - firer->getStatus().getPosition());
	BOOST_AUTO(orient, firer->getStatus().getFrame());
	BOOST_AUTO(dir, orient.globalToLocalRelative(displacement));
	BOOST_AUTO(left, Point<sint32>(0, 1, 0));
	BOOST_AUTO(right, Point<sint32>(0, -1, 0));
	BOOST_AUTO(cmp, cos((22.5 * M_PI) * dir.length()/ 180));
	if(dir.innerProduct(left) > cmp or dir.innerProduct(right) > cmp) {
		status.setTargetDirection(displacement);
		return true;
		}
	else return false;
	}

void Laserator::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16) {
	server::world->addBeam(new LaserBeam(firer, status));
	}

LaserBeam::LaserBeam(const Ref<LayeredUnit>& source, const WeaponStatus& status): Beam(source->getStatus().getPosition(), status.getTargetDirection(), source, server::world->getTimeNow(), 3) {}

void LaserBeam::onInteractUnit(double, const Ref<LayeredUnit>& unit, bool leaving) {
	if (not leaving) unit->damage(HitPoints(1));
	}

bool TorpedoLauncher::aim(const Ref<LayeredUnit>& firer, WeaponStatus& status, const WeaponOrders& orders) {
	if(server::world->getTimeNow() - lastFire < 100) return false; /* does not overflow unless time travel */
	BOOST_AUTO(displacement ,orders.getTargetPosition() - firer->getStatus().getPosition());
        BOOST_AUTO(orient, firer->getStatus().getFrame());
        BOOST_AUTO(dir, orient.globalToLocalRelative(displacement));
        BOOST_AUTO(forward, Point<sint32>(1, 0, 0));
        BOOST_AUTO(cmp, cos((22.5 * M_PI) * dir.length()/ 180));
        if(dir.innerProduct(forward) > cmp) {
		BOOST_AUTO(sr, SphereRegion<sint32>(Position(), 2.5 * CM));
                status.setTargetDirection(sr.truncateToFit(displacement));
                return true;
                }
        else return false;
        }

void TorpedoLauncher::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16) {
	/** \todo permit clients to fire torpedoes out of the plane, if they really want to */
	BOOST_AUTO(angle, Orientation(rotation_anticlockwise, atan2(status.getTargetDirection().y, status.getTargetDirection().x) / M_PI * 18000));
	BOOST_AUTO(tp, status.getTargetDirection() + firer->getStatus().getPosition());
	Point<double> tv = status.getTargetDirection();
	tv.normalise();
	tv *= 30000;
	BOOST_AUTO(u, LayeredUnit::spawn(PlayerId::fromString("0"), /* All torpedoes are neutral */
	server::world->getUniverse()->getUnitTypeId("torp"), Frame(tp, angle), Velocity(tv))); 
	BOOST_AUTO(f, boost::shared_ptr<TorpedoFuse>(new TorpedoFuse())); /* can't use TorpedoFuse::Ptr as ambiguous */
	u->insertLayer(f);
	lastFire = server::world->getTimeNow();
	server::world->addFuse(f, lastFire + 33); /* Fuse of 1/3 of a turn, in which the torpedo should move 10cm and the ship 6.66 cm, leaving them well over 2.5cm apart */
	Debug("Launched torpedo at time " << lastFire);
	}

void TorpedoDetonator::incrementState() {
	UnitMask::incrementState();
	BOOST_AUTO(local, SphereRegion<sint32>(getOuterUnit()->getStatus().getPosition(), 2.5 * CM));
	BOOST_AUTO(lbb, local.getBoundingBox());
	/** \todo use the actual sphere for intersections, not the bounding box */
	/* Too complicated for BOOST_AUTO, it seems */
	ISpatial::Result targets = server::world->getSpatialIndex()->findIntersecting(lbb, gameObject_unit);
	for(BOOST_AUTO(t, targets.begin()); t != targets.end(); t++)
		if(t->dynamicCast<LayeredUnit>()->getOwner()) {
			/* For now, torpedoes never detonate on each other - really, need some way to discriminate between torpedoes in the same salvo. Perhaps using a wide but very short detection box? */
			/* Detonate the torpedo, damaging only one ship - for balance reasons, and can be justified with nanotech or something */
			Debug("Detonating torpedo with id " << int(this->getOuterUnit()->getId()) << " at time " << server::world->getTimeNow() << ", targetting player: " << t->dynamicCast<LayeredUnit>()->getOwner() << ", unit: " << t->dynamicCast<LayeredUnit>()->getId());
			t->dynamicCast<LayeredUnit>()->damage(HitPoints(200));
			this->getOuterUnit()->kill();
			return;
			}
	}

void TorpedoFuse::expire(FuseToken) {
	this->getOuterUnit()->insertLayer(TorpedoDetonator::Ptr(new TorpedoDetonator()));
	this->getOuterUnit()->removeLayer(this);
	}

extern "C" {
	MODULE_API Weapon* spawn_laser(const WeaponType* type) { return new Laserator(type); }
	MODULE_API Script* create_script() { return new SagScript(); }
	MODULE_API PlayerData* create_player() { return new SagPlayerData(); }
	MODULE_API Weapon* spawn_torpedo(const WeaponType* type) { return new TorpedoLauncher(type); }
	}
