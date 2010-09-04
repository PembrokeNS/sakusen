#include <sakusen/libsakusen-global.h>

#include <iostream>

namespace sakusen {

LIBSAKUSEN_API std::ostream& debugStream = std::cout;
LIBSAKUSEN_API std::ostream& errorStream = std::cerr;

}

