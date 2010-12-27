#ifndef LIBSAKUSEN__ORDERTYPE_H
#define LIBSAKUSEN__ORDERTYPE_H

#include <sakusen/global.h>

#include <iosfwd>

namespace sakusen {

/** \brief Enumeration of types of order which can be give to units.
 *
 * Each type has its own subclass of OrderData which carries the relevant data
 * for that order */
enum OrderType {
  orderType_none,
  /** Set the unit's velocity to this value */
  orderType_setVelocity,
  /** Move the unit to this position */
  orderType_move,
  /** Set the unit's angular velocity */
  orderType_setAngularVelocity,
  /** Rotate the unit to a given orientation */
  orderType_orient,
  /** Untarget a weapon */
  orderType_targetNone,
  /** Target a weapon at a Position */
  orderType_targetPosition,
  /** Target a weapon at a Frame (e.g. for creating a
   * building with the Position and Orientation of the frame) */
  orderType_targetFrame,
  /** "Target" a weapon at a number, for weapons that do things like modify the unit */
  orderType_targetNumber,
  /** Target a weapon at a unit.  Note that this will always be a friendly unit;
   * enemy units are always SensorReturns */
  orderType_targetUnit,
  /** Target a weapon at a SensorReturns */
  orderType_targetSensorReturns,
  /** Must be the last value */
  orderType_max
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, OrderType);

}

#endif // LIBSAKUSEN__ORDERTYPE_H

