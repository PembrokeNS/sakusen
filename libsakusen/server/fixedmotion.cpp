#include "fixedmotion.h"

#include "layeredunit.h"
#include "unitcorneriterator.h"

namespace sakusen {
namespace server {

void FixedMotion::incrementState(LayeredUnit& unit)
{
  /* Ensure at ground level. */
  /** \bug This only really needs to be done when the unit first appears and
   * when the ground level changes, not every tick.
   *
   * \bug This allows the unit to exist upside-down and underground. */
  const IHeightfield& hf = world->getMap()->getHeightfield();
  bool okay = true;

  for (UnitCornerIterator corner(unit, true), end; corner != end; ++corner) {
    Position cornerPos = *corner;
    const sint32 height = hf.getHeightAt(cornerPos);
    const sint32 heightDifference = height - cornerPos.z;
    if (heightDifference > maxHeightDifference ||
        heightDifference < -maxHeightDifference) {
      okay = false;
      break;
    }
  }

  if (!okay) {
    unit.kill();
  }
}

}}

