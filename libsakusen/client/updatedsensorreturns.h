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
      *this = to;
    }
};

}}

#endif // UPDATEDSENSORRETURNS_H

