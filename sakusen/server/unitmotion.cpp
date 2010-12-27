#include <sakusen/server/unitmotion.h>

#include <sakusen/server/groundmotion.h>
#include <sakusen/server/fixedmotion.h>

namespace sakusen {
namespace server {

UnitMotion* UnitMotion::create(const String& type)
{
  /** \todo Choose which to construct in a more Cunning fashion so as to allow
   * derived classes to be gotten out of dlopened modules? */
  if (type == "ground") {
    return new GroundMotion();
  } else if (type == "fixed") {
    return new FixedMotion();
  } else {
    SAKUSEN_FATAL("unexpected UnitMotion type");
  }
}

}}

