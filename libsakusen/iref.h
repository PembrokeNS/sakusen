#ifndef IREF_H
#define IREF_H

#include "ireferent.h"
#include "maskedptr.h"

namespace sakusen {

class IRef {
  protected:
    IRef() {}
    IRef(const IRef&) {}
  public:
    virtual ~IRef() {}
    virtual void invalidate() = 0;
    virtual operator MaskedPtr<IReferent>() const = 0;
    virtual IRef* newCopy() const = 0;
};

}

#endif // IREF_H

