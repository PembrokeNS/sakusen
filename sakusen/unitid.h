#ifndef LIBSAKUSEN__UNITID_H
#define LIBSAKUSEN__UNITID_H

#include <sakusen/global.h>
#include <sakusen/idbase.h>

namespace sakusen {

/** \brief Unique identifier for a unit amongst units of one player. */
class UnitId : public IdBase<uint32, UnitId> {
};

}

#endif // LIBSAKUSEN__UNITID_H

