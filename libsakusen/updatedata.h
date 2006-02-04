#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include "libsakusen-global.h"
#include "changeownerreason.h"
#include "updatetype.h"
#include "ordercondition.h"
#include "oarchive.h"

namespace sakusen {

class Unit;
class Order;

class LIBSAKUSEN_API UpdateData {
  /* Abstract class - all constructors must be protected */
  protected:
    UpdateData() {}
    UpdateData(const UpdateData&) {}
  public:
    virtual ~UpdateData() {}
    virtual UpdateData* newCopy() const = 0;
    virtual UpdateType getType() const = 0;
    virtual void store(OArchive& out) const = 0;
};

class LIBSAKUSEN_API UnitRemovedUpdateData : public UpdateData {
  private:
    UnitRemovedUpdateData();
  public:
    UnitRemovedUpdateData(uint32 id, changeOwnerReason reason);
    ~UnitRemovedUpdateData() {}
  private:
    uint32 id;
    changeOwnerReason reason;
  public:
    UpdateData* newCopy() const { return new UnitRemovedUpdateData(*this); }
    UpdateType getType() const { return updateType_unitRemoved; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API UnitAddedUpdateData : public UpdateData {
  private:
    UnitAddedUpdateData();
  public:
    UnitAddedUpdateData(const Unit* unit, changeOwnerReason reason);
    ~UnitAddedUpdateData() {}
  private:
    const Unit* unit;
    changeOwnerReason reason;
  public:
    UpdateData* newCopy() const { return new UnitAddedUpdateData(*this); }
    UpdateType getType() const { return updateType_unitAdded; }
    void store(OArchive& out) const;
    inline const Unit* getUnit() const { return unit; }
    inline changeOwnerReason getReason() const { return reason; }
};

class LIBSAKUSEN_API OrderAcceptedUpdateData : public UpdateData {
  private:
    OrderAcceptedUpdateData();
  public:
    OrderAcceptedUpdateData(uint32 unitId, OrderCondition condition);
    ~OrderAcceptedUpdateData() {}
  private:
    uint32 unitId;
    OrderCondition condition;
  public:
    UpdateData* newCopy() const { return new OrderAcceptedUpdateData(*this); }
    UpdateType getType() const { return updateType_orderAccepted; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API OrderCompletedUpdateData : public UpdateData {
  private:
    OrderCompletedUpdateData();
  public:
    OrderCompletedUpdateData(uint32 unitId, OrderCondition condition);
    ~OrderCompletedUpdateData() {}
  private:
    uint32 unitId;
    OrderCondition condition;
  public:
    inline uint32 getUnitId() const { return unitId; }
    inline OrderCondition getCondition() const { return condition; }
    UpdateData* newCopy() const { return new OrderCompletedUpdateData(*this); }
    UpdateType getType() const { return updateType_orderCompleted; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API OrderQueuedUpdateData : public UpdateData {
  private:
    OrderQueuedUpdateData();
  public:
    OrderQueuedUpdateData(
        uint32 unitId,
        const Order* order,
        OrderCondition condition
      );
    ~OrderQueuedUpdateData() {}
  private:
    uint32 unitId;
    const Order* order;
    OrderCondition condition;
  public:
    UpdateData* newCopy() const { return new OrderQueuedUpdateData(*this); }
    UpdateType getType() const { return updateType_orderQueued; }
    void store(OArchive& out) const;
};

}

#endif // UPDATEDATA_H

