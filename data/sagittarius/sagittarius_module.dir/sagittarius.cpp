#include <boost/typeof/typeof.hpp>

#include "weapontype.h"
#include "ref.h"
#include "layeredunit.h"
#include "weapon.h"
#include "beam.h"
#include "completeworld.h"
#include "sphereregion.h"
#include "rectangleregion.h"
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
		HitPoints damage;
		LaserBeam(const Ref<LayeredUnit>& source, const WeaponStatus& status, HitPoints d);
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
		bool aim(const Ref<LayeredUnit>&, WeaponStatus&, const WeaponOrders& orders) { return orders.getTargetType() == weaponTargetType_number and orders.getTargetNumber() <= 100; }
		void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders& orders , uint16) { (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData))->attack = orders.getTargetNumber(); }
		uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
	};

class DefenseSetter: public Weapon {
	public:
		DefenseSetter(const WeaponType* type) : Weapon(type) {}
		bool aim(const Ref<LayeredUnit>&, WeaponStatus&, const WeaponOrders& orders)  { return orders.getTargetType() == weaponTargetType_number and orders.getTargetNumber() <= 100; }
		void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders& orders, uint16) { (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData))->defense = orders.getTargetNumber(); }
		uint32 getProjectileSpeed() const { return 0; } //Unused as this is non-ballisticm
	};

class SpeedSetter: public Weapon {
	public:
		SpeedSetter(const WeaponType* type) : Weapon(type) {}
		bool aim(const Ref<LayeredUnit>&, WeaponStatus&, const WeaponOrders& orders)  { return orders.getTargetType() == weaponTargetType_number and orders.getTargetNumber() <= 100; }
		void onFire(const Ref<LayeredUnit>& firer, const WeaponStatus&, WeaponOrders& orders, uint16)  { (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData))->speed = orders.getTargetNumber(); }
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
		HitPoints damage;
		void incrementState();
		TorpedoDetonator(HitPoints d): UnitMask() { damage = d; }
	};

class TorpedoFuse: public UnitMask, public Fuse {
	public:
		HitPoints damage;
		void expire(FuseToken);
		TorpedoFuse(HitPoints d): UnitMask(), Fuse() { damage = d; }
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

class DefenseLayer: public UnitMask {
	public:
		HitPoints getMaxHitPoints() const { return HitPoints(1000 * (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(getOwner())->playerData))->defense); }
	};

class SpeedLayer: public UnitMask {
	public:
		Rectangle<sint16> rar;
		Region<sint16>::ConstPtr ar;
		Region<sint16>::ConstPtr vr;
		Region<sint32>::ConstPtr avr;
		Region<sint16>::ConstPtr getPossibleAccelerations() const { return ar; }
		Region<sint16>::ConstPtr getPossibleVelocities() const { return vr; }
		Region<sint32>::ConstPtr getPossibleAngularVelocities() const { return avr; }
		SpeedLayer(uint16 s);
	};

SpeedLayer::SpeedLayer(uint16 s):
	UnitMask(),
	rar(-2, -2, s, 2),
	ar(new RectangleRegion<sint16>(rar)),
	vr(new SphereRegion<sint16>(Velocity(), 200 * s)),
	avr(new SphereRegion<sint32>(AngularVelocity(), s / 2))
{
}

void Laserator::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16) {
	server::world->addBeam(new LaserBeam(firer, status, HitPoints(boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData)->attack)));
	}

LaserBeam::LaserBeam(const Ref<LayeredUnit>& source, const WeaponStatus& status, HitPoints d): Beam(source->getStatus().getPosition(), status.getTargetDirection(), source, server::world->getTimeNow(), 3), damage(d) {}

void LaserBeam::onInteractUnit(double, const Ref<LayeredUnit>& unit, bool leaving) {
	if (not leaving) unit->damage(this->damage);
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

bool FleetCreator::aim(const Ref<LayeredUnit>& firer, WeaponStatus& status, const WeaponOrders& orders) {
	/* can't use BOOST_AUTO because , in the middle */
	boost::shared_ptr<SagPlayerData> pd = boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData);
	if(pd->attack + pd->defense + pd->speed != 100) return false;
	BOOST_AUTO(displacement ,orders.getTargetPosition() - firer->getStatus().getPosition());
	BOOST_AUTO(orient, firer->getStatus().getFrame());
	BOOST_AUTO(sr, SphereRegion<sint32>(Position(), 5 * CM));
	status.setTargetDirection(sr.truncateToFit(displacement));
	return true;
	}

void FleetCreator::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16) {
	/** \todo Require ships to be created in the plane */
	for(int i=0; i<1500; i++) {
		BOOST_AUTO(u, LayeredUnit::spawn(firer->getOwner(), server::world->getUniverse()->getUnitTypeId("ship"), Frame(status.getTargetDirection() + firer->getStatus().getPosition(), firer->getStatus().getFrame().getOrientation()), Velocity(), HitPoints(1000 * (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData))->defense)));
		BOOST_AUTO(f, boost::shared_ptr<DefenseLayer>(new DefenseLayer()));
		BOOST_AUTO(g, boost::shared_ptr<SpeedLayer>(new SpeedLayer(boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData)->speed)));
		u->insertLayer(f);
		u->insertLayer(g);
		u->setDirty();
	}
	firer->kill();
	}

void TorpedoLauncher::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16) {
	BOOST_AUTO(angle, Orientation(rotation_anticlockwise, atan2(status.getTargetDirection().y, status.getTargetDirection().x) / M_PI * 18000));
	BOOST_AUTO(tp, status.getTargetDirection() + firer->getStatus().getPosition());
	Point<double> tv = status.getTargetDirection();
	tv.normalise();
	tv *= 30000;
	BOOST_AUTO(u, LayeredUnit::spawn(PlayerId::fromString("0"), /* All torpedoes are neutral */
	server::world->getUniverse()->getUnitTypeId("torp"), Frame(tp, angle), Velocity(tv))); 
	BOOST_AUTO(f, boost::shared_ptr<TorpedoFuse>(new TorpedoFuse(HitPoints(200 * (boost::dynamic_pointer_cast<SagPlayerData, PlayerData>(server::world->getPlayerPtr(firer->getOwner())->playerData))->attack)))); /* can't use TorpedoFuse::Ptr as ambiguous */
	u->insertLayer(f);
	lastFire = server::world->getTimeNow();
	server::world->addFuse(f, lastFire + 33); /* Fuse of 1/3 of a turn, in which the torpedo should move 10cm and the ship 6.66 cm, leaving them well over 2.5cm apart */
	SAKUSEN_DEBUG("Launched torpedo at time " << lastFire);
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
			SAKUSEN_DEBUG("Detonating torpedo with id " << int(this->getOuterUnit()->getId()) << " at time " << server::world->getTimeNow() << ", targetting player: " << t->dynamicCast<LayeredUnit>()->getOwner() << ", unit: " << t->dynamicCast<LayeredUnit>()->getId());
			t->dynamicCast<LayeredUnit>()->damage(this->damage);
			this->getOuterUnit()->kill();
			return;
			}
	}

void TorpedoFuse::expire(FuseToken) {
	this->getOuterUnit()->insertLayer(TorpedoDetonator::Ptr(new TorpedoDetonator(damage)));
	this->getOuterUnit()->removeLayer(this);
	}

extern "C" {
	MODULE_API Weapon* spawn_laser(const WeaponType* type) { return new Laserator(type); }
	MODULE_API Script* create_script() { return new SagScript(); }
	MODULE_API PlayerData* create_player() { return new SagPlayerData(); }
	MODULE_API Weapon* spawn_torpedo(const WeaponType* type) { return new TorpedoLauncher(type); }
	MODULE_API Weapon* spawn_attack(const WeaponType* type) { return new AttackSetter(type); }
	MODULE_API Weapon* spawn_defense(const WeaponType* type) { return new DefenseSetter(type); }
	MODULE_API Weapon* spawn_speed(const WeaponType* type) { return new SpeedSetter(type); }
	MODULE_API Weapon* spawn_fleet(const WeaponType* type) { return new FleetCreator(type); }
	}
