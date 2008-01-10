#include "libsakusen-resources-global.h"

namespace sakusen {
namespace resources {

LIBSAKUSEN_RESOURCES_API pcrecpp::RE filenameRegex("[-a-z0-9_.]+");
LIBSAKUSEN_RESOURCES_API pcrecpp::RE pathRegex("[-a-z0-9_./]+");

}}

