#ifndef UPDATEDUNIT_H
#define UPDATEDUNIT_H

#include "completeunit.h"
#include "updatedata.h"
#include "unitorders.h"

namespace sakusen {
namespace client {

/** \brief A player-owned ::Unit that can accept information about its orders
 * and status when they arrive over the network.
 */
class UpdatedUnit : public CompleteUnit {
  private:
    UpdatedUnit();
  public:
    UpdatedUnit(const CompleteUnit& copy) :
      CompleteUnit(copy), orders(), altered(false)
    {}
    virtual ~UpdatedUnit() {}
  private:
    /** This Unit's current order status. */
    UnitOrders orders;
    /** Whether this Unit has already been altered this tick */
    bool altered;
  public:
    /** \name Accessors */
    /*@{*/
    inline const UnitOrders &getUnitOrders() const {
      return orders;
    }
    /*@}*/
    void orderQueued(const OrderQueuedUpdateData& data);
    void orderAccepted(const OrderAcceptedUpdateData& data);
    void orderCompleted(const OrderCompletedUpdateData& data);
    void alter(const CompleteUnit&);
    void incrementState();
};

}}

#endif // UPDATEDUNIT_H

