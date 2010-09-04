#ifndef LIBSAKUSEN__RESOURCETYPE_H
#define LIBSAKUSEN__RESOURCETYPE_H

#include <sakusen/global.h>

#include <iosfwd>

namespace sakusen {

/** \brief Enumerates the types which can be sought through a ResourceInterface
 */
enum ResourceType {
  resourceType_universe,
  resourceType_mapTemplate,
  resourceType_source,
  resourceType_module,
  resourceType_replay,
  resourceType_replayIndex
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, ResourceType resource);

}

#endif // LIBSAKUSEN__RESOURCETYPE_H

