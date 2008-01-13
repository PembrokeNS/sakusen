#include "game/smartunitfactory.h"

#include "smartunit.h"

using namespace sakusen;
using namespace sakusen::client;

namespace tedomari {
namespace game {

UpdatedUnit::Ptr SmartUnitFactory::create(const CompleteUnit& unit) const {
  return UpdatedUnit::Ptr(new SmartUnit(unit));
}

}}

