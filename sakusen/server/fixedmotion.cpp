#include <sakusen/server/fixedmotion.h>

#include <sakusen/server/layeredunit.h>
#include <sakusen/unitcorneriterator.h>

namespace sakusen {
namespace server {

void FixedMotion::incrementState(LayeredUnit& unit)
{
  /* Ensure at ground level. */
  /** \bug This only really needs to be done when the unit first appears and
   * when the ground level changes, not every tick.
   *
   * \bug This allows the unit to exist upside-down and underground. */
  const Map* map = world->getMap();
  const IHeightfield& hf = map->getHeightfield();
  bool okay = true;

  for (UnitCornerIterator corner(unit, true), end; corner != end; ++corner) {
    Position cornerPos = *corner;
    map->resolvePosition(cornerPos, cornerPos);
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

