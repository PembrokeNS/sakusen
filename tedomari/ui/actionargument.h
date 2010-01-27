#ifndef TEDOMARI__UI__ACTIONARGUMET_H
#define TEDOMARI__UI__ACTIONARGUMET_H

#include "tedomari-global.h"

#include "numbertarget.h"
#include "point.h"
#include "orientation.h"
#include "ref.h"
#include "updatedunit.h"
#include "ui/actiontarget.h"

namespace tedomari {
namespace ui {

/* Note that it's critical that the order of the types in the variant
 * correspond to the order of the values in the enum, so that variant::which()
 * can be used to get the type. */
typedef boost::variant<
    boost::blank,
    ActionTarget,
    String,
    sakusen::Frame,
    sakusen::Ref<sakusen::client::UpdatedUnit>,
    std::vector<sakusen::Ref<sakusen::client::UpdatedUnit> >,
    sakusen::NumberTarget
  > ActionArgument;

enum ActionParameterType {
  actionParameterType_none,
  actionParameterType_target,
  actionParameterType_stringFromSet,
  actionParameterType_frame,
  actionParameterType_unit,
  actionParameterType_unitSet,
  actionParameterType_number,
  actionParameterType_max /* Must be last */
};

}}

#endif // TEDOMARI__UI__ACTIONARGUMET_H

