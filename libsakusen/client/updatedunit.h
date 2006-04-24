#ifndef UPDATEDUNIT_H
#define UPDATEDUNIT_H

#include "completeunit.h"
#include "updatedata.h"
#include "unitorders.h"

namespace sakusen {
namespace client {

class UpdatedUnit : public CompleteUnit {
  private:
    UpdatedUnit();
  public:
    UpdatedUnit(const CompleteUnit& copy) :
      CompleteUnit(copy), orders(), altered(false)
    {}
    virtual ~UpdatedUnit() {}
  private:
    UnitOrders orders;
    /** Whether this Unit has already been altered this tick */
    bool altered;
  public:
    void orderQueued(const OrderQueuedUpdateData& data);
    void orderAccepted(const OrderAcceptedUpdateData& data);
    void orderCompleted(const OrderCompletedUpdateData& data);
    void alter(const CompleteUnit&);
    void incrementState();
};

}}

#endif // UPDATEDUNIT_H

