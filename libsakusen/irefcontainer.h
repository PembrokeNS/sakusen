#ifndef IREFCONTAINER_H
#define IREFCONTAINER_H

#include "iref.h"

namespace sakusen {

/** \brief An interface for a container which manages objects which should be
 * handled through Refs
 *
 * Any container which handles the memory management for objects which are
 * handled through Refs needs to implement this interface so that these Refs
 * can register and unregister themselves as they are constructed and
 * destroyed.
 *
 * Note that implementing this does not imply any of the properties of the STL
 * concept Container.  It need not be possible to add and remove elements, etc.
 * Observe SensorReturns, which implements IRefContainer, but manages Refs to
 * but a single object (it's member unit).
 */
class IRefContainer {
  protected:
    IRefContainer() {}
    IRefContainer(const IRefContainer&) {}
  public:
    virtual ~IRefContainer() {}
    virtual void registerRef(IRef*) const = 0;
    virtual void unregisterRef(IRef*) const = 0;
};

}

#endif // IREFCONTAINER_H

