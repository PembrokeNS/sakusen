#ifndef UPDATEDSENSORRETURNS_H
#define UPDATEDSENSORRETURNS_H

namespace sakusen {
namespace client {

/** \brief SensorReturns which can be updated as new information arrives.
 *
 * This class inherits from SensorReturns, and thus encapsulates everything
 * known to this client's player about some other player's unit through
 * sensors, but it also provides for updating that information as new
 * information is received from the server.
 */
class UpdatedSensorReturns : public SensorReturns {
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
    }
};

} /* back into namespace sakusen */

namespace client {

/** \brief Function object to extract the id from an UpdatedSensorReturns */ 
struct UpdatedSensorReturnsIder {
  SensorReturnsId operator()(const Ref<UpdatedSensorReturns>& s) const {
    return s->getId();
  }
};

}}

#endif // UPDATEDSENSORRETURNS_H

