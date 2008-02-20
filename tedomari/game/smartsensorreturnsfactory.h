#ifndef TEDOMARI__GAME__SMARTSENSORRETURNSFACTORY_H
#define TEDOMARI__GAME__SMARTSENSORRETURNSFACTORY_H

#include "sensorreturnsfactory.h"

namespace tedomari {
namespace game {

class SmartSensorReturnsFactory : public sakusen::client::SensorReturnsFactory {
  public:
    virtual sakusen::client::UpdatedSensorReturns::Ptr create(
        const sakusen::SensorReturns&
      ) const;
};

}}

#endif // TEDOMARI__GAME__SMARTSENSORRETURNSFACTORY_H

