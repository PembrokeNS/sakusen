#ifndef LIBSAKUSEN_SERVER__UNITMOTION_H
#define LIBSAKUSEN_SERVER__UNITMOTION_H

#include "libsakusen-server-global.h"

#include <boost/utility.hpp>

#include "unitstatus.h"
#include "unitorders.h"
#include "unitlayer.h"

namespace sakusen {
namespace server {

/** \brief Abstract class encapsulating unit movement requirements.
 *
 * This class can be specialized for each different mode of movement: air
 * units, ground units, boats, submarines, and more exotic things. */
class LIBSAKUSEN_SERVER_API UnitMotion : private boost::noncopyable {
  public:
    static UnitMotion* create(const String& type);
  protected:
    UnitMotion() {}
  public:
    virtual inline ~UnitMotion() = 0;

    /** \brief Compute appropriate velocity and angular velocity.
     *
     * Each subclass of UnitMotion should overload this method to define the
     * motion rules for a unit.
     *
     * The method should examine the unit's orders, and alter the velocity and
     * angular velocity of the unit's status according to what the unit's
     * orders are requesting.  It should not actually alter the unit's position
     * or orientation &mdash; that is done in LayeredUnit::incrementState
     * immediately after the call to this method.
     *
     * It is acceptable for the unit to be killed during the call to this
     * method.
     */
    virtual void incrementState(LayeredUnit&) = 0;
};

inline UnitMotion::~UnitMotion() {}

}}

#endif // LIBSAKUSEN_SERVER__UNITMOTION_H

