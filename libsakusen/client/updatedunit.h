#ifndef UPDATEDUNIT_H
#define UPDATEDUNIT_H

#include "completeunit.h"
#include "updatedata.h"

namespace sakusen {
namespace client {

class UpdatedUnit : public CompleteUnit {
  private:
    UpdatedUnit();
  public:
    UpdatedUnit(const CompleteUnit& copy) :
      CompleteUnit(copy), updated(false)
    {}
    virtual ~UpdatedUnit() {}
  private:
    /** Whether this Unit has already been updated this tick */
    bool updated;

    void clearOrders();
  public:
    void orderQueued(const OrderQueuedUpdateData& data);
    void orderAccepted(const OrderAcceptedUpdateData& data);
    void orderCompleted(const OrderCompletedUpdateData& data);
    void incrementState();
};

}}

#endif // UPDATEDUNIT_H

