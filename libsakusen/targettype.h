#ifndef TARGETTYPE_H
#define TARGETTYPE_H

namespace sakusen {

/** \brief Enumerations of the types of target which can determine the linear
 * motion of a unit */
enum LinearTargetType {
  linearTargetType_none,
  /** The unit is trying to reach a given position */
  linearTargetType_position,
  /** The unit is trying to achieve a given velocity (probably 0, in most
   * cases) */
  linearTargetType_velocity
};

/** \brief Enumerations of the types of target which can determine the
 * rotational motion of a unit */
enum RotationalTargetType {
  rotationalTargetType_none,
  /** The unit is trying to reach a given orientation */
  rotationalTargetType_orientation,
  /** The unit is trying to achieve a given angular velocity */
  rotationalTargetType_angularVelocity
};

}

#endif // TARGETTYPE_H

