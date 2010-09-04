#include <sakusen/resources/global.h>

#include <boost/xpressive/regex_primitives.hpp>

namespace sakusen {
namespace resources {

namespace xp = boost::xpressive;

LIBSAKUSEN_RESOURCES_API xp::sregex filenameRegex =
  +xp::set[xp::range('a', 'z')|xp::range('0','9')|'-'|'_'|'.'];
LIBSAKUSEN_RESOURCES_API xp::sregex pathRegex =
  +xp::set[xp::range('a', 'z')|xp::range('0','9')|'-'|'_'|'.'|'/'];

}}

