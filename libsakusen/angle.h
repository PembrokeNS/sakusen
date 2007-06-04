#ifndef ANGLE_H
#define ANGLE_H

#include "libsakusen-global.h"
#include "point.h"

namespace sakusen {

/** \brief Typedef for angle, in degrees */
typedef sint16 Angle;

/** \brief Typedef for angular speed, in degrees per tick */
typedef sint16 AngularSpeed;

/** \brief Typedef for anglar velocity, in some units or other...
 * 
 * \todo Decide what units this is in */
typedef Point<sint16> AngularVelocity;

}

#endif // ANGLE_H

