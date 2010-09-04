#ifndef LIBSAKUSEN_CLIENT__UNITFACTORY_H
#define LIBSAKUSEN_CLIENT__UNITFACTORY_H

#include "updatedunit.h"

namespace sakusen {
namespace client {

class UnitFactory : private boost::noncopyable {
  protected:
    UnitFactory() {}
  public:
    virtual inline ~UnitFactory() = 0;
    virtual UpdatedUnit::Ptr create(const CompleteUnit&) const = 0;
};

UnitFactory::~UnitFactory() {}

}}

#endif // LIBSAKUSEN_CLIENT__UNITFACTORY_H

