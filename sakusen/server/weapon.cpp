#include <sakusen/server/weapon.h>

#include <sakusen/mathsutils.h>
#include <sakusen/server/completeworld.h>

using namespace sakusen;
using namespace sakusen::server;

Weapon::Weapon(const WeaponType* t) :
  type(t->getId())
{
}

/** \brief Target the weapon according to given orders
 *
 * \param firer  The unit firing
 * \param status The status of the weapon
 * \param orders The orders applicable to the weapon.  It will be true that
 *               orders.isTargetValid()
 * \return true iff the weapon is sufficiently on-target that we wish to
 *         open fire
 *
 * The default implementation targets as if the weapon fired a Ballistic
 * with speed the return value of projectileSpeed.  A subclass which fires
 * anything other than a Ballistic must override this with an appropriate
 * method.
 *
 * Roughly the following semantics should be used:
 * Determine the target by consulting the \p orders parameter.
 * Alter the \p status parameter appropriately
 * (e.g. if the weapon has slow horizontal tracking like a Vulcan, or if there
 * have been complex computations to transform a Position target into a
 * direction).  Return true iff it is reasonable for the Weapon to open fire
 * given its present status.  If you return true then there will may (subject
 * to resource / time constraints) be a subsequent call to onFire this tick.
 * So, onFire should be able to cope with any \p status that might be left by
 * this function.
 *
 * \todo Maybe this implementation should be moved to a subclass
 * BallisticWeapon, and other similar be created to aid the creation of
 * modules.
 *
 * \bug Observations suggest that this method doesn't actually work properly
 * (it tends to aim short).  It needs unit testing.
 */
bool Weapon::aim(
    const Ref<LayeredUnit>& firer,
    WeaponStatus& status,
    const WeaponOrders& orders
  )
{
  const Point<sint32>& pos = orders.getTargetPosition();
  /** \bug For now we ignore the target velocity and we ignore the fact that
   * the weapon is not actually at the firer unit position */
  Point<sint32> displacement = pos - firer->getStatus().getPosition();
  /** \bug Using floating point arithmetic */
  double height = displacement.z;
  displacement.z = 0;
  double range = displacement.length();
  double gravity = world->getMap()->getGravity();
  double projectileSpeed = getProjectileSpeed();

  /* compute the coefficients in the quadratic for tan(theta) */
  double a = gravity * pow(range,2);
  double b = -2 * range * pow(projectileSpeed, 2);
  double c = 2 * height * pow(projectileSpeed, 2) + gravity * pow(range, 2);

  double tanTheta;
  /* We want the smaller root for a shorter path time.  This is
   * always first return value. */
  boost::tie(tanTheta, boost::tuples::ignore) =
    mathsUtils_solveQuadratic(a, b, c);

  if (isnan(tanTheta)) {
    return false;
  }
  
  double theta = atan(tanTheta);

  Point<double> initialVelocity(
      cos(theta) * projectileSpeed * displacement.x / range,
      cos(theta) * projectileSpeed * displacement.y / range,
      sin(theta) * projectileSpeed
    );

  /** \todo Worry about weapon rotation speeds, etc. */
  /** \todo Inform clients */
  status.setTargetDirection(Point<sint16>(initialVelocity.truncate()));
  return true;
}

/** \brief Increment the state of the Weapon for a new tick.
 *
 * This consults the Weapon's orders and uses them to call aim.  If the Weapon
 * is on target, it activates it, otherwise it deactivates it.  If the Weapon
 * is active, it calls WeaponStatus::incrementState.  If that call indicates
 * that the Weapon should fire, it calls onFire.
 */
void Weapon::incrementState(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  /* Obtain the status */
  UnitStatus& unitStatus = firer->getStatus();
  assert(weaponIndex < unitStatus.getWeaponsStatus().size());
  WeaponStatus& status = unitStatus.getWeaponsStatus()[weaponIndex];
  
  /* Obtain the orders */
  WeaponOrders& orders =
    firer->getOrders().getWeaponsOrders()[weaponIndex];

  if (!orders.isTargetValid()) {
    status.deactivate();
  } else {
    if (aim(
          firer, status, orders
        )) {
      status.activate();
    } else {
      status.deactivate();
    }
  }

  Player* player = server::world->getPlayerPtr(firer->getOwner());

  if (status.incrementState(type, *player)) {
    /* Return value of true means that there were enough resources to fire */
    onFire(firer, status, orders, weaponIndex);
  }
}

