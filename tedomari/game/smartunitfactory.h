#ifndef TEDOMARI__GAME__SMARTUNITFACTORY_H
#define TEDOMARI__GAME__SMARTUNITFACTORY_H

#include <sakusen/client/updatedunit.h>
#include <sakusen/client/unitfactory.h>

namespace tedomari {
namespace game {

class SmartUnitFactory : public sakusen::client::UnitFactory {
  public:
    virtual sakusen::client::UpdatedUnit::Ptr create(
        const sakusen::CompleteUnit&
      ) const;
};

}}

#endif // TEDOMARI__GAME__SMARTUNITFACTORY_H

