#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include "libsakusen-global.h"
#include "changeownerreason.h"
#include "updatetype.h"
#include "order.h"
#include "ordercondition.h"
#include "oarchive.h"
#include "completeunit.h"
#include "sensorreturns.h"
#include "quadratic.h"

namespace sakusen {

class Order;

class LIBSAKUSEN_API UpdateData {
  public:
    typedef boost::shared_ptr<UpdateData> Ptr;
    typedef boost::shared_ptr<const UpdateData> ConstPtr;
  protected:
    /* Abstract class - all constructors must be protected */
    UpdateData() {}
    UpdateData(const UpdateData&) {}
  public:
    virtual ~UpdateData() {}
    virtual UpdateType getType() const = 0;
    virtual void store(OArchive& out) const = 0;
};

class LIBSAKUSEN_API UnitRemovedUpdateData : public UpdateData {
  private:
    UnitRemovedUpdateData();
  public:
    UnitRemovedUpdateData(uint32 i, changeOwnerReason r) :
      UpdateData(), id(i), reason(r) {}
    UnitRemovedUpdateData(IArchive&);
    ~UnitRemovedUpdateData() {}
  private:
    uint32 id;
    changeOwnerReason reason;
  public:
    UpdateType getType() const { return updateType_unitRemoved; }
    void store(OArchive& out) const;
    uint32 getId() const { return id; }
    changeOwnerReason getReason() const { return reason; }
};

class LIBSAKUSEN_API UnitAddedUpdateData : public UpdateData {
  private:
    UnitAddedUpdateData();
  public:
    UnitAddedUpdateData(
        changeOwnerReason r,
        const Ref<const ICompleteUnit>& u
      ) :
      UpdateData(), reason(r), unit(u) {}
    UnitAddedUpdateData(IArchive&, const Universe::ConstPtr*);
    ~UnitAddedUpdateData() {}
  private:
    changeOwnerReason reason;
    CompleteUnit unit;
  public:
    UpdateType getType() const { return updateType_unitAdded; }
    void store(OArchive& out) const;
    inline changeOwnerReason getReason() const { return reason; }
    inline const CompleteUnit& getUnit() const { return unit; }
};

class LIBSAKUSEN_API UnitAlteredUpdateData : public UpdateData {
  private:
    UnitAlteredUpdateData();
  public:
    UnitAlteredUpdateData(const Ref<const ICompleteUnit>& u) :
      UpdateData(), unit(u) {}
    UnitAlteredUpdateData(IArchive&, const Universe::ConstPtr*);
    ~UnitAlteredUpdateData() {}
  private:
    CompleteUnit unit;
  public:
    UpdateType getType() const { return updateType_unitAltered; }
    void store(OArchive& out) const;
    inline const CompleteUnit& getUnit() const { return unit; }
};

class LIBSAKUSEN_API OrderAcceptedUpdateData : public UpdateData {
  private:
    OrderAcceptedUpdateData();
  public:
    OrderAcceptedUpdateData(uint32 uI, OrderCondition c) :
      UpdateData(), unitId(uI), condition(c) {}
    OrderAcceptedUpdateData(IArchive&);
    ~OrderAcceptedUpdateData() {}
  private:
    uint32 unitId;
    OrderCondition condition;
  public:
    inline uint32 getUnitId() const { return unitId; }
    inline OrderCondition getCondition() const { return condition; }
    UpdateType getType() const { return updateType_orderAccepted; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API OrderCompletedUpdateData : public UpdateData {
  private:
    OrderCompletedUpdateData();
  public:
    OrderCompletedUpdateData(uint32 uI, OrderCondition c) :
      UpdateData(), unitId(uI), condition(c) {}
    OrderCompletedUpdateData(IArchive&);
    ~OrderCompletedUpdateData() {}
  private:
    uint32 unitId;
    OrderCondition condition;
  public:
    inline uint32 getUnitId() const { return unitId; }
    inline OrderCondition getCondition() const { return condition; }
    UpdateType getType() const { return updateType_orderCompleted; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API OrderQueuedUpdateData : public UpdateData {
  private:
    OrderQueuedUpdateData();
  public:
    OrderQueuedUpdateData(uint32 uI, const Order& o, OrderCondition c) :
      UpdateData(), unitId(uI), order(o), condition(c) {}
    OrderQueuedUpdateData(IArchive&, const PlayerID*);
    ~OrderQueuedUpdateData() {}
  private:
    uint32 unitId;
    Order order;
    OrderCondition condition;
  public:
    inline uint32 getUnitId() const { return unitId; }
    inline OrderCondition getCondition() const { return condition; }
    inline const Order& getOrder() const { return order; }
    UpdateType getType() const { return updateType_orderQueued; }
    void store(OArchive& out) const;
};

class LIBSAKUSEN_API SensorReturnsRemovedUpdateData : public UpdateData {
  private:
    SensorReturnsRemovedUpdateData();
  public:
    SensorReturnsRemovedUpdateData(SensorReturnsID i) :
      UpdateData(), id(i) {}
    SensorReturnsRemovedUpdateData(IArchive&);
    ~SensorReturnsRemovedUpdateData() {}
  private:
    SensorReturnsID id;
  public:
    UpdateType getType() const { return updateType_sensorReturnsRemoved; }
    void store(OArchive& out) const;
    SensorReturnsID getId() const { return id; }
};

class LIBSAKUSEN_API SensorReturnsAddedUpdateData : public UpdateData {
  private:
    SensorReturnsAddedUpdateData();
  public:
    SensorReturnsAddedUpdateData(const Ref<const ISensorReturns> r) :
      UpdateData(), returns(r) {}
    SensorReturnsAddedUpdateData(IArchive&, const Universe::ConstPtr*);
    ~SensorReturnsAddedUpdateData() {}
  private:
    SensorReturns returns;
  public:
    UpdateType getType() const { return updateType_sensorReturnsAdded; }
    void store(OArchive& out) const;
    inline const SensorReturns& getSensorReturns() const { return returns; }
};

class LIBSAKUSEN_API SensorReturnsAlteredUpdateData : public UpdateData {
  private:
    SensorReturnsAlteredUpdateData();
  public:
    SensorReturnsAlteredUpdateData(const Ref<const ISensorReturns>& r) :
      UpdateData(), returns(r) {}
    SensorReturnsAlteredUpdateData(IArchive&, const Universe::ConstPtr*);
    ~SensorReturnsAlteredUpdateData() {}
  private:
    SensorReturns returns;
  public:
    UpdateType getType() const { return updateType_sensorReturnsAltered; }
    void store(OArchive& out) const;
    inline const SensorReturns& getSensorReturns() const { return returns; }
};

class LIBSAKUSEN_API BallisticRemovedUpdateData : public UpdateData {
  private:
    BallisticRemovedUpdateData();
  public:
    BallisticRemovedUpdateData(uint32 i) :
      UpdateData(), id(i) {}
    BallisticRemovedUpdateData(IArchive&);
    ~BallisticRemovedUpdateData() {}
  private:
    uint32 id;
  public:
    UpdateType getType() const { return updateType_ballisticRemoved; }
    void store(OArchive& out) const;
    uint32 getId() const { return id; }
};

class LIBSAKUSEN_API BallisticAddedUpdateData : public UpdateData {
  private:
    BallisticAddedUpdateData();
  public:
    BallisticAddedUpdateData(uint32 i, const Quadratic& p) :
      UpdateData(), id(i), path(p) {}
    BallisticAddedUpdateData(IArchive&);
    ~BallisticAddedUpdateData() {}
  private:
    uint32 id;
    Quadratic path;
  public:
    UpdateType getType() const { return updateType_ballisticAdded; }
    void store(OArchive& out) const;
    uint32 getId() const { return id; }
    inline const Quadratic& getPath() const { return path; }
};

}

#endif // UPDATEDATA_H

