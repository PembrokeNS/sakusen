#ifndef TARGETTYPE_H
#define TARGETTYPE_H

namespace sakusen {

enum LinearTargetType {
  linearTargetType_none,
  linearTargetType_position,
  linearTargetType_velocity
};

enum RotationalTargetType {
  rotationalTargetType_none,
  rotationalTargetType_orientation,
  rotationalTargetType_angularVelocity
};

}

#endif // TARGETTYPE_H

