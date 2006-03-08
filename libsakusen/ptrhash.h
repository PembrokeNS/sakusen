#ifndef PTRHASH_H
#define PTRHASH_H

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

