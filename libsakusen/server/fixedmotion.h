#ifndef LIBSAKUSEN_SERVER__FIXEDMOTION_H
#define LIBSAKUSEN_SERVER__FIXEDMOTION_H

#include "unitmotion.h"

namespace sakusen {
namespace server {

class FixedMotion : public UnitMotion {
  public:
    void incrementState(LayeredUnit&);
};

}}

#endif // LIBSAKUSEN_SERVER__FIXEDMOTION_H

