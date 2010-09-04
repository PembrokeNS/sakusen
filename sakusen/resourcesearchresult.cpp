#include "resourcesearchresult.h"

#include "libsakusen-global.h"

#include <ostream>

using namespace sakusen;

std::ostream& sakusen::operator<<(
    std::ostream& output,
    ResourceSearchResult resource
  )
{
  switch(resource) {
    #define CASE(type) case type: output << #type; break;
    CASE(resourceSearchResult_success)
    CASE(resourceSearchResult_ambiguous)
    CASE(resourceSearchResult_notFound)
    CASE(resourceSearchResult_error)
    CASE(resourceSearchResult_notSupported)
    #undef CASE
    default:
      output << "resourceType_" << uint32(resource);
      break;
  }
  return output;
}

