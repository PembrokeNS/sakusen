#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

#include "libsakusen-global.h"

#include <iosfwd>

namespace sakusen {

/** \brief Enumerates the types which can be sought through a ResourceInterface
 */
enum ResourceType {
  resourceType_universe,
  resourceType_mapTemplate
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, ResourceType resource);

}

#endif // RESOURCETYPE_H

