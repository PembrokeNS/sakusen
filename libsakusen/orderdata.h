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
    SetVelocityOrderData(const Velocity& t) : OrderData(), target(t) {}
    SetVelocityOrderData(IArchive& in) : OrderData() { in >> target; }
    ~SetVelocityOrderData() {};
  private:
    Velocity target;
  public:
    const Velocity& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_setVelocity; }
    void store(OArchive&) const;
};

/** \brief Base for all OrderData classes which target a weapon.
 *
 * This is an intermediate class which allows extraction of the weaponIndex
 * without worrying about exactly what type of targeting is being performed.
 */
class LIBSAKUSEN_API TargetWeaponOrderData : public OrderData {
  public:
    virtual uint16 getWeaponIndex() const = 0;
};

class LIBSAKUSEN_API TargetPositionOrderData : public TargetWeaponOrderData {
  private:
    TargetPositionOrderData();
  public:
    TargetPositionOrderData(uint16 wI, const Point<sint32>& t) :
      TargetWeaponOrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetPositionOrderData(IArchive& in) : TargetWeaponOrderData() {
      in >> weaponIndex >> target;
    }
    ~TargetPositionOrderData() {};
  private:
    uint16 weaponIndex;
    Point<sint32> target;
  public:
    uint16 getWeaponIndex(void) const { return weaponIndex; }
    const Point<sint32>& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_targetPosition; }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API TargetPositionOrientationOrderData :
  public TargetWeaponOrderData {
  private:
    TargetPositionOrientationOrderData();
  public:
    TargetPositionOrientationOrderData(
        uint16 wI,
        const std::pair<Position, Orientation>& t
      ) :
      TargetWeaponOrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetPositionOrientationOrderData(IArchive& in) : TargetWeaponOrderData() {
      in >> weaponIndex >> target;
    }
    ~TargetPositionOrientationOrderData() {};
  private:
    uint16 weaponIndex;
    std::pair<Position, Orientation> target;
  public:
    uint16 getWeaponIndex(void) const { return weaponIndex; }
    const std::pair<Position, Orientation>& getTarget() const { return target; }
    OrderType getType(void) const {
      return orderType_targetPositionOrientation;
    }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API TargetUnitOrderData : public TargetWeaponOrderData {
  public:
    TargetUnitOrderData(uint16 wI, const Ref<ICompleteUnit>& t) :
      TargetWeaponOrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetUnitOrderData(IArchive& in, const PlayerID* player) :
      TargetWeaponOrderData()
    {
      in >> weaponIndex;
      target = Ref<ICompleteUnit>::load(in, player);
    }
    ~TargetUnitOrderData() {};
  private:
    uint16 weaponIndex;
    Ref<ICompleteUnit> target;
  public:
    uint16 getWeaponIndex(void) const { return weaponIndex; }
    const Ref<ICompleteUnit>& getTarget(void) const { return target; }
    OrderType getType(void) const { return orderType_targetUnit; }
    void store(OArchive&) const;
};

class LIBSAKUSEN_API TargetSensorReturnsOrderData :
  public TargetWeaponOrderData {
  private:
    TargetSensorReturnsOrderData();
  public:
    TargetSensorReturnsOrderData(uint16 wI, const Ref<ISensorReturns>& t) :
      TargetWeaponOrderData(),
      weaponIndex(wI),
      target(t)
    {}
    TargetSensorReturnsOrderData(IArchive& in, const PlayerID* player) :
      TargetWeaponOrderData()
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

}

#endif // ORDERDATA_H

