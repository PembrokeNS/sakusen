#ifndef SENSORRETURNSID_H
#define SENSORRETURNSID_H

#include "idwrapper.h"

namespace sakusen {

class SensorReturnsId : public IdWrapper<uint32, SensorReturnsId> {
};

}

namespace __gnu_cxx {

template<>
struct hash<sakusen::SensorReturnsId> {
  private:
    hash<sakusen::SensorReturnsId::internal_type> intHasher;
  public:
    size_t operator()(const sakusen::SensorReturnsId i) const {
      return intHasher(i);
    }
};

}

#endif // SENSORRETURNSID_H

