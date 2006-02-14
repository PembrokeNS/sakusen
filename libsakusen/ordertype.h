#ifndef ORDERTYPE_H
#define ORDERTYPE_H

#include <iosfwd>

namespace sakusen {

/* Enumeration of types of order which can be give to units.  Each type has its
 * own subclass of OrderData which carries the relevant data for that order */

enum OrderType {
  orderType_none,
  orderType_setVelocity, /* Set the Unit's velocity to this value */
  orderType_move /* Move the unit to this position */
};

std::ostream& operator<<(std::ostream& output, OrderType);

}

#endif // ORDERTYPE_H

