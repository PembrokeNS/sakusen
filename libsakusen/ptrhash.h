#ifndef PTRHASH_H
#define PTRHASH_H

//need definition of size_t
#include <cstring>


namespace sakusen {

template<typename T>
class PtrHash {
  public:
    inline size_t operator()(T* const& p) const {
      return reinterpret_cast<size_t>(p);
    }
};

}

#endif // PTRHASH_H

