#ifndef UI__ACTIONTARGET_H
#define UI__ACTIONTARGET_H

#include <boost/variant.hpp>

#include "point.h"

/* Note that it's critical that the order of the types in the variant
 * correspond to the order of the values in the enum, so that variant::which()
 * can be used to get the type. */
typedef boost::variant<
    boost::blank,
    sakusen::Position
  > ActionTarget;

enum ActionTargetType {
  actionTargetType_none,
  actionTargetType_position
};

#endif // UI__ACTIONTARGET_H

