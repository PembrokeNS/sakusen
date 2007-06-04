#ifndef UI__ACTIONARGUMET_H
#define UI__ACTIONARGUMET_H

#include "point.h"
#include "orientation.h"
#include "ref.h"
#include "updatedunit.h"
#include "ui/actiontarget.h"

/* Note that it's critical that the order of the types in the variant
 * correspond to the order of the values in the enum, so that variant::which()
 * can be used to get the type. */
typedef boost::variant<
    boost::blank,
    ActionTarget,
    String,
    std::pair<sakusen::Position, sakusen::Orientation>,
    sakusen::Ref<sakusen::client::UpdatedUnit>
  > ActionArgument;

enum ActionParameterType {
  actionParameterType_none,
  actionParameterType_target,
  actionParameterType_stringFromSet,
  actionParameterType_positionOrientation,
  actionParameterType_unit
};

#endif // UI__ACTIONARGUMET_H
