#ifndef UNITID_H
#define UNITID_H

#include "libsakusen-global.h"
#include "idbase.h"

namespace sakusen {

/** \brief Unique identifier for a unit amongst units of one player. */
class UnitId : public IdBase<uint32, UnitId> {
};

}

#endif // UNITID_H

