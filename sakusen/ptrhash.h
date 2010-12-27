#ifndef LIBSAKUSEN__PTRHASH_H
#define LIBSAKUSEN__PTRHASH_H

#include <sakusen/global.h>

namespace sakusen {

template<typename T>
class PtrHash {
  public:
    inline size_t operator()(T* const& p) const {
      return reinterpret_cast<size_t>(p);
    }
};

}

#endif // LIBSAKUSEN__PTRHASH_H

