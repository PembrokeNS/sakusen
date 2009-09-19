#include <boost/typeof/typeof.hpp>

#include "weapontype.h"
#include "ref.h"
#include "layeredunit.h"
#include "weapon.h"
#include "beam.h"
#include "completeworld.h"

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

bool Laserator::aim(const Ref<LayeredUnit>& firer, WeaponStatus& status, const WeaponOrders& orders) {
	BOOST_AUTO(displacement ,orders.getTargetPosition() - firer->getStatus().getPosition());
	BOOST_AUTO(orient, firer->getStatus().getFrame());
	BOOST_AUTO(dir, orient.globalToLocalRelative(displacement));
	dir.normalise();
	BOOST_AUTO(left, Point<sint32>(1, 0, 0));
	BOOST_AUTO(right, Point<sint32>(-1, 0, 0));
	BOOST_AUTO(cmp, cos((22.5 * M_PI) / 180));
	if(dir.innerProduct(left) >= cmp or dir.innerProduct(right) >= cmp) {
		status.setTargetDirection(displacement);
		return true;
		}
	else return false;
	}

void Laserator::onFire(const Ref<LayeredUnit>& firer, const WeaponStatus& status, WeaponOrders&, uint16 weaponIndex) {
	server::world->addBeam(new LaserBeam(firer, status));
	}

LaserBeam::LaserBeam(const Ref<LayeredUnit>& source, const WeaponStatus& status): Beam(source->getStatus().getPosition(), status.getTargetDirection(), source, server::world->getTimeNow(), 3) {}

void LaserBeam::onInteractUnit(double position, const Ref<LayeredUnit>& unit, bool leaving) {
	if (not leaving) unit->damage(HitPoints(1));
	}

extern "C" {
	MODULE_API Weapon* spawn_laser(const WeaponType* type) { return new Laserator(type); }
	MODULE_API Script* create_script() { return new SagScript(); }
	MODULE_API Weapon* spawn_torpedo(const WeaponType* type) { return 0; }

	}
