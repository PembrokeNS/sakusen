#include <sakusen/resourcetype.h>

#include <sakusen/global.h>

#include <ostream>

using namespace std;

using namespace sakusen;

ostream& sakusen::operator<<(ostream& output, ResourceType resource)
{
  switch(resource) {
    #define CASE(type) \
    case resourceType_##type:                        \
      output << "resourceType_"#type; break;
    CASE(universe)
    CASE(mapTemplate)
    CASE(source)
    CASE(module)
    #undef CASE
    default:
      output << "resourceType_" << uint32(resource);
      break;
  }
  return output;
}

