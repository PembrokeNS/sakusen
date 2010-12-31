#ifndef LIBSAKUSEN__UPDATETYPE_H
#define LIBSAKUSEN__UPDATETYPE_H

#include <sakusen/global.h>

#include <iosfwd>

namespace sakusen {

/** \brief Enumeration of types of message sent from server to client to
 * inform of an update to
 * the game state.
 *
 * What arguments each takes is given in its description.
 *
 * \note Current serialization implementation limits us to 256 types. */
enum UpdateType {
  /** Dummy value because mistakes will most probably lead to a value of zero */
  updateType_none,
  /** Remove unit.
   *
   * UnitId, changeOwnerReason */
  updateType_unitRemoved,
  /** Add unit.
   *
   * CompleteUnit, changeOwnerReason */
  updateType_unitAdded,
  /** Alter existing unit.
   *
   * CompleteUnit */
  updateType_unitAltered,
  /** Order accepted from client.
   *
   * UnitId, Order */
  updateType_orderAccepted,
  /** Remove SensorReturns.
   *
   * SensorReturnsId */
  updateType_sensorReturnsRemoved,
  /** Add new SensorReturns.
   *
   * SensorReturns */
  updateType_sensorReturnsAdded,
  /** Alter existing SensorReturns.
   *
   * SensorReturns */
  updateType_sensorReturnsAltered,
  /** Remove Ballistic.
   *
   * id */
  updateType_ballisticRemoved,
  /** Add new Ballistic
   *
   * id, Quadratic */
  updateType_ballisticAdded
};

LIBSAKUSEN_API std::ostream& operator<<(
    std::ostream& output,
    UpdateType update
  );

}

#endif // LIBSAKUSEN__UPDATETYPE_H

