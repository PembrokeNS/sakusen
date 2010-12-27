#ifndef LIBSAKUSEN_SERVER__FIXEDMOTION_H
#define LIBSAKUSEN_SERVER__FIXEDMOTION_H

#include <sakusen/server/unitmotion.h>

namespace sakusen {
namespace server {

class FixedMotion : public UnitMotion {
  public:
    /** \brief Maximum difference in height between a bottom corner of the unit
     * and the ground (in dex).
     *
     * If the unit is positioned such that one of its bottom corners is further
     * than this above or below the ground, then it will be killed.
     *
     * \bug Arbitrary constant. */
    static const sint32 maxHeightDifference = 100;

    void incrementState(LayeredUnit&);
};

}}

#endif // LIBSAKUSEN_SERVER__FIXEDMOTION_H

