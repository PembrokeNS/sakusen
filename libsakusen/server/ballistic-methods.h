#ifndef BALLISTIC_METHODS_H
#define BALLISTIC_METHODS_H

#include "ballistic.h"
#include "completeworld.h"

namespace sakusen {
/* Not an error - this really isn't in namespace sakusen::server, but it is
 * part of libsakusen-server */

inline void RefHandler<server::Ballistic>::registerRef(
    Ref<server::Ballistic>* ref
  ) const
{
  server::world->registerRef(ref);
}

inline void RefHandler<server::Ballistic>::unregisterRef(
    Ref<server::Ballistic>* ref
  ) const
{
  server::world->unregisterRef(ref);
}

/** \bug We can't serialize because we're using pointers only as Ballistic IDs.
 * At present that doesn't matter, but it will when we come to implement
 * savegames.  The following methods are declared inline, so it will become
 * obvious that they are needed as soon as they are used anywhere. */
#if 0
inline server::Ballistic* RefHandler<server::Ballistic>::extract(
    IArchive& archive
  ) const
{
  uint32 id;
  archive >> id;
  return server::world->getBallistic(id);
}

inline void RefHandler<server::Ballistic>::insert(
    OArchive& archive,
    server::Ballistic* ret
  ) const {
  archive << ret->getServerId();
}
#endif

}

#endif // BALLISTIC_METHODS_H

