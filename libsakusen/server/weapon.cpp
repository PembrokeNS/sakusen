#include "weapon.h"

#include "mathsutils.h"

using namespace boost;
using namespace sakusen;
using namespace sakusen::server;

Weapon::Weapon(const Weapon& copy) :
  type(copy.type)
{
}

Weapon::Weapon(const WeaponType* t) :
  type(t->getId())
{
}

/** \brief Target the weapon according to given orders
 *
 * \param firer The unit firing
 * \param status The status of the weapon
 * \param orders The orders applicable to the weapon
 * \return true iff the weapon is sufficiently on-target that we wish to
 * open fire
 *
 * The default implementation targets as if the weapon fired a Ballistic
 * with speed the return value of projectileSpeed */
bool Weapon::aim(
    const Ref<LayeredUnit>& firer,
    WeaponStatus* status,
    const WeaponOrders& orders
  )
{
  const Point<sint32>& pos = orders.getTargetPosition();
  /** \bug For now we ignore the target velocity and we ignore the fact that
   * the weapon is not actually at the firer unit position */
  Point<sint32> displacement = pos - firer->getStatus()->getPosition();
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
  tie(tanTheta, tuples::ignore) = mathsUtils_solveQuadratic(a, b, c);

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
  status->setTargetDirection(Point<sint16>(initialVelocity.truncate()));
  return true;
}

void Weapon::incrementState(const Ref<LayeredUnit>& firer, uint16 weaponIndex)
{
  /* Obtain the status */
  UnitStatus* unitStatus = firer->getStatus();
  assert(weaponIndex < unitStatus->getWeaponsStatus().size());
  WeaponStatus* status = &unitStatus->getWeaponsStatus()[weaponIndex];
  
  /* Obtain the orders */
  const WeaponOrders& orders =
    firer->getOrders().getWeaponsOrders()[weaponIndex];

  if (!orders.isTargetValid()) {
    status->deactivate();
  } else {
    if (aim(
          firer, status, orders
        )) {
      status->activate();
    } else {
      status->deactivate();
    }
  }

  if (status->incrementState(type)) {
    /* Return value of true means that there were enough resources to fire */
    onFire(firer, weaponIndex);
  }
}

