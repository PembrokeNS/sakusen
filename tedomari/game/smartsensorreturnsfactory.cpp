#include "game/smartsensorreturnsfactory.h"

#include "game/smartsensorreturns.h"

using namespace sakusen;
using namespace sakusen::client;

namespace tedomari {
namespace game {

UpdatedSensorReturns::Ptr SmartSensorReturnsFactory::create(
    const SensorReturns& sensorReturns
  ) const {
  return UpdatedSensorReturns::Ptr(new SmartSensorReturns(sensorReturns));
}

}}

