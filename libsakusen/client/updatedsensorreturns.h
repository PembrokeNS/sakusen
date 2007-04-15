#ifndef UPDATEDSENSORRETURNS_H
#define UPDATEDSENSORRETURNS_H

namespace sakusen {
namespace client {

class UpdatedSensorReturns : public SensorReturns {
  private:
    UpdatedSensorReturns();
  public:
    UpdatedSensorReturns(const SensorReturns& copy) :
      SensorReturns(copy)
    {}
    virtual ~UpdatedSensorReturns() {}
    
    void alter(const SensorReturns& to) {
      this->SensorReturns::operator=(to);
    }
};

} /* back into namespace sakusen */

template<>
class SerializationHandler<client::UpdatedSensorReturns> {
  public:
    typedef void loadArgument;
};

namespace client {

struct UpdatedSensorReturnsIDer {
  uint32 operator()(const Ref<UpdatedSensorReturns>& s) const {
    return s->getId();
  }
};

}}

#endif // UPDATEDSENSORRETURNS_H

