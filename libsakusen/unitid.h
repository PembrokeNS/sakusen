#ifndef UNITID_H
#define UNITID_H

#include "libsakusen-global.h"
#include "idbase.h"

namespace sakusen {

/** \brief Unique identifier for a unit amongst units of one player. */
class UnitId : public IdBase<uint32, UnitId> {
};

}

namespace __gnu_cxx {

template<>
struct hash<sakusen::UnitId> {
  private:
    hash<sakusen::UnitId::internal_type> intHasher;
  public:
    size_t operator()(const sakusen::UnitId i) const {
      return intHasher(i);
    }
};

}

#endif // UNITID_H

