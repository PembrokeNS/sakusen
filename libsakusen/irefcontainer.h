#ifndef IREFCONTAINER_H
#define IREFCONTAINER_H

#include "iref.h"

namespace sakusen {

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

