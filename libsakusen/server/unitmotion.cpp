#include "unitmotion.h"

#include "groundmotion.h"
#include "fixedmotion.h"

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
    Fatal("unexpected UnitMotion type");
  }
}

}}

