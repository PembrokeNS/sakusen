#ifndef ORDERDATA_H
#define ORDERDATA_H

#include "libsakusen-global.h"

#include "point.h"
#include "oarchive.h"
#include "iarchive.h"
#include "sensorreturns.h"
#include "ordertype.h"
#include "ref.h"

namespace sakusen {

class LIBSAKUSEN_API OrderData {
  public:
    typedef boost::shared_ptr<OrderData> Ptr;
    typedef boost::shared_ptr<const OrderData> ConstPtr;
  protected:
    /* abstract class - all constructors must be protected */
    OrderData();
    OrderData(const OrderData& copy);
    OrderData& operator=(const OrderData&) { return *this; }
  public:
    virtual ~OrderData() {};
    virtual OrderType getType(void) const = 0;
    virtual void store(OArchive&) const = 0;
};

class LIBSAKUSEN_API MoveOrderData : public OrderData {
  private:
    MoveOrderData();
  public:
    MoveOrderData(const Point<sint32>& t) : OrderData(), target(t) {}
    MoveOrderData(IArchive& in) : OrderData() { in >> target; }
    ~MoveOrderData() {};
  private:
    Point<sint32> target;
  public:
    const Point<sint32>& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_move; }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API SetVelocityOrderData : public OrderData {
  private:
    SetVelocityOrderData();
  public:
    SetVelocityOrderData(const Point<sint16>& t) : OrderData(), target(t) {}
    SetVelocityOrderData(IArchive& in) : OrderData() { in >> target; }
    ~SetVelocityOrderData() {};
  private:
    Point<sint16> target;
  public:
    Point<sint32> getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_setVelocity; }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API TargetSensorReturnsOrderData : public OrderData {
  private:
    TargetSensorReturnsOrderData();
  public:
    TargetSensorReturnsOrderData(uint16 wI, const Ref<ISensorReturns>& t) :
      OrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetSensorReturnsOrderData(IArchive& in, const PlayerID* player) :
      OrderData()
    {
      in >> weaponIndex;
      target = Ref<ISensorReturns>::load(in, player);
    }
    ~TargetSensorReturnsOrderData() {};
  private:
    uint16 weaponIndex;
    Ref<ISensorReturns> target;
  public:
    uint16 getWeaponIndex(void) const { return weaponIndex; }
    const Ref<ISensorReturns>& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_targetSensorReturns; }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API TargetPointOrderData : public OrderData {
  private:
    TargetPointOrderData();
  public:
    TargetPointOrderData(uint16 wI, const Point<sint32>& t) :
      OrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetPointOrderData(IArchive& in) : OrderData() {
      in >> weaponIndex >> target;
    }
    ~TargetPointOrderData() {};
  private:
    uint16 weaponIndex;
    Point<sint32> target;
  public:
    uint16 getWeaponIndex(void) const { return weaponIndex; }
    const Point<sint32>& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_targetPoint; }
    void store(OArchive&) const;
};

}

#endif // ORDERDATA_H

