#ifndef UNITMASK_H
#define UNITMASK_H

#include "unitlayer.h"

namespace sakusen {
namespace server {

/** \brief A UnitLayer which forms a mask, altering the unit in some way
 *
 * This class is intended to be the base of any UnitLayer other than UnitCore.
 * It implements all the methods of UnitLayer with versions that don't change
 * the unit at all; you must override those which you wish to do something
 * non-trivial.
 */
class UnitMask : public UnitLayer {
};

}}

#endif // UNITMASK_H

