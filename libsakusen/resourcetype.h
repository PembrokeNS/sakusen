#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

#include <iosfwd>

/** \brief Enumerates the types which can be sought through a ResourceInterface
 */
enum ResourceType {
  resourceType_universe,
  resourceType_map
};

std::ostream& operator<<(std::ostream& output, ResourceType resource);

#endif // RESOURCETYPE_H

