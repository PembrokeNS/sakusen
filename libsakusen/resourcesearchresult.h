#ifndef RESOURCESEARCHRESULT_H
#define RESOURCESEARCHRESULT_H

#include <iosfwd>

namespace sakusen {

/** \brief Enumeration of possible results of searching for a resource
 *
 * When search for a resource through a ResourceInterface, one of the following
 * enumeration values will be returned to describe the result of the search
 */
enum ResourceSearchResult {
  resourceSearchResult_success, /**< The resource was found and returned */
  resourceSearchResult_ambiguous, /**< The name specified applied to more than
                                    one resource */
  resourceSearchResult_notFound, /**< No resource of that name could be found */
  resourceSearchResult_error /**< The resource was found, but an error occured
                               while reading it */
};

std::ostream& operator<<(std::ostream& output, ResourceSearchResult resource);

}

#endif // RESOURCESEARCHRESULT_H

