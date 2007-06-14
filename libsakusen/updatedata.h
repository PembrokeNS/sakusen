#ifndef UPDATEDATA_H
#define UPDATEDATA_H

#include "libsakusen-global.h"
#include "changeownerreason.h"
#include "updatetype.h"
#include "order.h"
#include "oarchive.h"
#include "completeunit.h"
#include "sensorreturns.h"
#include "quadratic.h"
#include "clientballisticid.h"

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
    UnitRemovedUpdateData(UnitID i, changeOwnerReason r) :
      UpdateData(), id(i), reason(r) {}
    UnitRemovedUpdateData(IArchive&);
    ~UnitRemovedUpdateData() {}
  private:
    UnitID id;
    changeOwnerReason reason;
  public:
    UpdateType getType() const { return updateType_unitRemoved; }
    void store(OArchive& out) const;
    UnitID getId() const { return id; }
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
    UnitAddedUpdateData(IArchive&, const DeserializationContext&);
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
    UnitAlteredUpdateData(IArchive&, const DeserializationContext&);
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
    OrderAcceptedUpdateData(UnitID uI, const Order& o) :
      UpdateData(), unitId(uI), order(o) {}
    OrderAcceptedUpdateData(IArchive&, const DeserializationContext&);
    ~OrderAcceptedUpdateData() {}
  private:
    UnitID unitId;
    Order order;
  public:
    inline UnitID getUnitId() const { return unitId; }
    inline const Order& getOrder() const { return order; }
    UpdateType getType() const { return updateType_orderAccepted; }
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
    SensorReturnsAddedUpdateData(IArchive&, const DeserializationContext&);
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
    SensorReturnsAlteredUpdateData(IArchive&, const DeserializationContext&);
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
    BallisticRemovedUpdateData(ClientBallisticID i) :
      UpdateData(), id(i) {}
    BallisticRemovedUpdateData(IArchive&);
    ~BallisticRemovedUpdateData() {}
  private:
    ClientBallisticID id;
  public:
    UpdateType getType() const { return updateType_ballisticRemoved; }
    void store(OArchive& out) const;
    ClientBallisticID getId() const { return id; }
};

class LIBSAKUSEN_API BallisticAddedUpdateData : public UpdateData {
  private:
    BallisticAddedUpdateData();
  public:
    BallisticAddedUpdateData(ClientBallisticID i, const Quadratic& p) :
      UpdateData(), id(i), path(p) {}
    BallisticAddedUpdateData(IArchive&);
    ~BallisticAddedUpdateData() {}
  private:
    ClientBallisticID id;
    Quadratic path;
  public:
    UpdateType getType() const { return updateType_ballisticAdded; }
    void store(OArchive& out) const;
    ClientBallisticID getId() const { return id; }
    inline const Quadratic& getPath() const { return path; }
};

}

#endif // UPDATEDATA_H

