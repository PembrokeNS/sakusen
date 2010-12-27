#ifndef LIBSAKUSEN_CLIENT__UPDATEDSENSORRETURNS_H
#define LIBSAKUSEN_CLIENT__UPDATEDSENSORRETURNS_H

#include <sakusen/client/global.h>

#include <sakusen/sensorreturns.h>

namespace sakusen {
namespace client {

/** \brief SensorReturns which can be updated as new information arrives.
 *
 * This class inherits from SensorReturns, and thus encapsulates everything
 * known to this client's player about some other player's unit through
 * sensors, but it also provides for updating that information as new
 * information is received from the server.
 */
class LIBSAKUSEN_CLIENT_API UpdatedSensorReturns : public SensorReturns {
  public:
    typedef boost::shared_ptr<UpdatedSensorReturns> Ptr;
    typedef boost::shared_ptr<const UpdatedSensorReturns> ConstPtr;
  private:
    UpdatedSensorReturns();

    bool altered_;
  public:
    UpdatedSensorReturns(const SensorReturns& copy) :
      SensorReturns(copy),
      altered_(true)
    {}
    virtual ~UpdatedSensorReturns() {}
    
    void alter(const SensorReturns& to);
    virtual void incrementState();
    virtual void altered() {}
    virtual void destroying() {}
    Ref<ISensorReturns> getRefToThis();
};

}}

#endif // LIBSAKUSEN_CLIENT__UPDATEDSENSORRETURNS_H

