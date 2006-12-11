#include "weapon.h"

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

/** \brief Target the weapon at an object with the given position and
 * velocity
 *
 * \param firer The unit firing
 * \param pos The target position
 * \param vel The target velocity
 * \return true iff the weapon is sufficiently on-target that we wish to
 * open fire
 *
 * The default implementation targets as if the weapon fired a Ballistic
 * with speed the return value of projectileSpeed */
bool Weapon::aimAt(
    const Ref<LayeredUnit>& firer,
    WeaponStatus* status,
    const Point<sint32>& pos,
    const Point<sint16>& /*vel*/
  )
{
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

  /* discriminant */
  double d = pow(b,2) - 4*a*c;

  if (d < 0) {
    return false;
  }

  /* Use some cunning quadratic solution method I found online */
  double q = -0.5*(b + sgn(b)*sqrt(d));

  /*QDebug("range=" << range << ", x1=" << q/a << ", x2=" << c/q);*/
  /* The roots are q/a and c/q.
   * We want the smaller root for a shorter path time.  I think that this is
   * always c/q, but I'm not sufficiently with it to be sure */
  double tanTheta = std::min(q/a, c/q);
  double theta = atan(tanTheta);

  Point<double> initialVelocity(
      cos(theta) * projectileSpeed * displacement.x / range,
      cos(theta) * projectileSpeed * displacement.y / range,
      sin(theta) * projectileSpeed
    );

  /** \todo Worry about weapon rotation speeds, etc. */
  /** \todo Inform clients */
  status->setDirection(Point<sint16>(initialVelocity.truncate()));
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
    /** \todo Account for the possibility that the weapon was targeted
     * manually, not at a given position */
    if (aimAt(
          firer, status, orders.getTargetPosition(), orders.getTargetVelocity()
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

