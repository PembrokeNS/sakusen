#ifndef LAYEREDUNIT_METHODS_H
#define LAYEREDUNIT_METHODS_H

#include "layeredunit.h"
#include "completeworld.h"

namespace sakusen {
/* Not an error - this really isn't in namespace sakusen::server, but it is
 * part of libsakusen-server */

inline void RefHandler<server::LayeredUnit>::registerRef(
    Ref<server::LayeredUnit>* ref
  ) const
{
  server::world->registerRef(ref);
}

inline void RefHandler<server::LayeredUnit>::unregisterRef(
    Ref<server::LayeredUnit>* ref
  ) const
{
  server::world->unregisterRef(ref);
}

/** \bug Serialization methods not yet implemented (they may well be needed for
 * savegames) */
#if 0
inline server::LayeredUnit* RefHandler<server::LayeredUnit>::extract(
    IArchive& archive
  ) const
{
}

inline void RefHandler<server::LayeredUnit>::insert(
    OArchive& archive,
    server::LayeredUnit* ret
  ) const {
}
#endif

}

#endif // LAYEREDUNIT_METHODS_H

