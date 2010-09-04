#ifndef LIBSAKUSEN__NULL_DELETER_H
#define LIBSAKUSEN__NULL_DELETER_H

#include <sakusen/libsakusen-global.h>

namespace sakusen {

struct null_deleter {
  void operator()(void const*) const {
  }
};

}

#endif // LIBSAKUSEN__NULL_DELETER_H

