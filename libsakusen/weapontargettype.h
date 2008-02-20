#ifndef WEAPONTARGETTYPE_H
#define WEAPONTARGETTYPE_H

namespace sakusen {

/** \brief Enumeration of types of target a weapon can have.
 *
 * Used in WeaponOrders. */
enum WeaponTargetType {
  weaponTargetType_none,
  weaponTargetType_direction,
  weaponTargetType_position,
  weaponTargetType_frame,
  weaponTargetType_unit,
  weaponTargetType_sensorReturns,
  weaponTargetType_max /* Must be last */
};

}

#endif // WEAPONTARGETTYPE_H

