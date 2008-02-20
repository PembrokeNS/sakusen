#ifndef TEDOMARI__GAME__SMARTSENSORRETURNS_H
#define TEDOMARI__GAME__SMARTSENSORRETURNS_H

namespace tedomari {
namespace game {

class SmartSensorReturns : public sakusen::client::UpdatedSensorReturns {
  public:
    SmartSensorReturns(const SensorReturns& sensorReturns) :
      UpdatedSensorReturns(sensorReturns)
    {}
};

}}

#endif // TEDOMARI__GAME__SMARTSENSORRETURNS_H

