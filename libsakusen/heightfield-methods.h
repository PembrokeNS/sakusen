#ifndef HEIGHTFIELD_METHODS_H
#define HEIGHTFIELD_METHODS_H

#include "heightfield.h"
#include "world.h"

namespace sakusen {

inline uint32 Heightfield::dexToSampleFloorX(sint32 x) const {
  return static_cast<uint32>(x - world->getMap()->left()) /
    xyResolution;
}

inline uint32 Heightfield::dexToSampleFloorY(sint32 y) const {
  return static_cast<uint32>(y - world->getMap()->bottom()) /
    xyResolution;
}

inline uint32 Heightfield::dexToSampleCeilX(sint32 x) const {
  uint32 offset = static_cast<uint32>(x - world->getMap()->left());
  if (0 == offset) {
    return 0;
  } else {
    return (offset-1)/xyResolution + 1;
  }
}

inline uint32 Heightfield::dexToSampleCeilY(sint32 y) const {
  uint32 offset = static_cast<uint32>(y - world->getMap()->bottom());
  if (0 == offset) {
    return 0;
  } else {
    return (offset-1)/xyResolution + 1;
  }
}

inline sint32 Heightfield::sampleToDexX(uint32 x) const {
  return world->getMap()->left() + x * xyResolution;
}

inline sint32 Heightfield::sampleToDexY(uint32 y) const {
  return world->getMap()->bottom() + y * xyResolution;
}

inline sint32 Heightfield::getApproxHeightAt(sint32 x, sint32 y) const {
  return getHeightAtSample(
      dexToSampleFloorX(x), dexToSampleFloorY(y)
    );
}

/** \brief Get an approximation to the height of the heightfield at a
 * given position.
 *
 * Given a position in space, returns an approximation to the
 * height on the vertical line through that position, in dex.  This
 * version is fast at the cost of accuracy; to get the exact height use
 * getHeightAt.
 *
 * \note \c p.z is irrelevant.
 */
inline sint32 Heightfield::getApproxHeightAt(const Position& p) const {
  return getApproxHeightAt(p.x, p.y);
}

}

#endif // HEIGHTFIELD_METHODS_H

