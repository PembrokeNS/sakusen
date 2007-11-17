#include "libsakusen-resources-global.h"

namespace sakusen {
namespace resources {

pcrecpp::RE filenameRegex("[-a-z0-9_.]+");
pcrecpp::RE pathRegex("[-a-z0-9_./]+");

}}

