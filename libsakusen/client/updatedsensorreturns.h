#ifndef UPDATEDSENSORRETURNS_H
#define UPDATEDSENSORRETURNS_H

#include "libsakusen-client-global.h"

#include "sensorreturns.h"

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
  public:
    UpdatedSensorReturns(const SensorReturns& copy) :
      SensorReturns(copy)
    {}
    virtual ~UpdatedSensorReturns() {}
    
    /** \brief Alter this to mimic the given SensorReturns
     *
     * \warning Don't try to achieve such an alteration with simple assignment.
     * It will go horribly wrong.
     */
    void alter(const SensorReturns& to) {
      this->SensorReturns::operator=(to);
      altered();
    }
    virtual void altered() {}
};

}}

#endif // UPDATEDSENSORRETURNS_H

