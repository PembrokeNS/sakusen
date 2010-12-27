#ifndef LIBSAKUSEN_SERVER__GROUNDMOTION_H
#define LIBSAKUSEN_SERVER__GROUNDMOTION_H

#include <sakusen/server/unitmotion.h>

namespace sakusen {
namespace server {

/** \brief Performs motion functions for a ground unit */
class GroundMotion : public UnitMotion {
  public:
    void incrementState(LayeredUnit&);
};

}}

#endif // LIBSAKUSEN_SERVER__GROUNDMOTION_H

