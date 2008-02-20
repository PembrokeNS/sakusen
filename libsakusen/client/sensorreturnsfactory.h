#ifndef LIBSAKUSEN_CLIENT__SENSORRETURNSFACTORY_H
#define LIBSAKUSEN_CLIENT__SENSORRETURNSFACTORY_H

#include "updatedsensorreturns.h"

namespace sakusen {
namespace client {

class SensorReturnsFactory : private boost::noncopyable {
  protected:
    SensorReturnsFactory() {}
  public:
    virtual inline ~SensorReturnsFactory() = 0;
    virtual UpdatedSensorReturns::Ptr create(const SensorReturns&) const = 0;
};

SensorReturnsFactory::~SensorReturnsFactory() {}

}}

#endif // LIBSAKUSEN_CLIENT__SENSORRETURNSFACTORY_H

