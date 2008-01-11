#ifndef LIBSAKUSEN_SERVER__UNITMOTION_H
#define LIBSAKUSEN_SERVER__UNITMOTION_H

#include "libsakusen-server-global.h"

#include <boost/utility.hpp>

#include "unitstatus.h"
#include "unitorders.h"
#include "unitlayer.h"

namespace sakusen {
namespace server {

/** \brief Abstract class encapsulating unit movement requirements.
 *
 * This class can be specialized for each different mode of movement: air
 * units, ground units, boats, submarines, and more exotic things. */
class UnitMotion : boost::noncopyable {
  public:
    static UnitMotion* create(const String& type);
  protected:
    UnitMotion() {}
  public:
    virtual inline ~UnitMotion() = 0;
    virtual void incrementState(LayeredUnit&) = 0;
};

inline UnitMotion::~UnitMotion() {}

}}

#endif // LIBSAKUSEN_SERVER__UNITMOTION_H

