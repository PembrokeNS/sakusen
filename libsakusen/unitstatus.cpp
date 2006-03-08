#include "unitstatus-methods.h"

#include "oarchive-methods.h"

using namespace sakusen;

UnitStatus::UnitStatus(
    UnitTypeID t,
    const Point<sint32>& p,
    const Orientation& o,
    const Point<sint16>& v,
    /* status stuff */
    HitPoints hP,
    bool rIA,
    bool sIA,
    const std::list<uint32>& su,
    const std::list<Weapon>& w,
    /* conditioned orders */
    Order ord[orderCondition_max],
    const Order& cO,
    /* current goals which have been set by orders */
    const LinearTargetType& lT,
    const Point<sint32>& tP,
    const Point<sint16>& tV,
    const RotationalTargetType& rT,
    const Orientation& tO,
    const AngularVelocity& tAV
  ) :
  type(t),
  position(p),
  orientation(o),
  velocity(v),
  hitPoints(hP),
  radarIsActive(rIA),
  sonarIsActive(sIA),
  subunits(su),
  weapons(w),
  currentOrder(cO),
  linearTarget(lT),
  targetPosition(tP),
  targetVelocity(tV),
  rotationalTarget(rT),
  targetOrientation(tO),
  targetAngularVelocity(tAV)
{
  for (int i=0; i<orderCondition_max; ++i) {
    orders[i] = ord[i];
  }
}

UnitStatus::UnitStatus(const IUnitStatus* copy) :
  type(copy->getType()),
  position(copy->getPosition()),
  orientation(copy->getOrientation()),
  velocity(copy->getVelocity()),
  hitPoints(copy->getHitPoints()),
  radarIsActive(copy->isRadarActive()),
  sonarIsActive(copy->isSonarActive()),
  currentOrder(copy->getCurrentOrder()),
  linearTarget(copy->getLinearTarget()),
  targetPosition(copy->getTargetPosition()),
  targetVelocity(copy->getTargetVelocity()),
  rotationalTarget(copy->getRotationalTarget()),
  targetOrientation(copy->getTargetOrientation()),
  targetAngularVelocity(copy->getTargetAngularVelocity())
{
  for (int i=0; i<orderCondition_max; ++i) {
    orders[i] = copy->getOrder(static_cast<OrderCondition>(i));
  }
  /* TODO: subunits, weapons */
}

UnitStatus::UnitStatus(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity,
    HitPoints startHitPoints,
    bool startRadarActive,
    bool startSonarActive
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  hitPoints(startHitPoints),
  radarIsActive(startRadarActive),
  sonarIsActive(startSonarActive),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
}

UnitStatus::UnitStatus(
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  assert(world != NULL);
  initializeWeapons(
      getTypePtr(), world->getUniverse()
    );
  hitPoints = getTypePtr()->getDynamicData().getMaxHitPoints();
}

UnitStatus::UnitStatus(
    const Universe* universe,
    const UnitTypeID& startType,
    const Point<sint32>& startPosition,
    const Orientation& startOrientation,
    const Point<sint16>& startVelocity
  ) :
  type(startType),
  position(startPosition),
  orientation(startOrientation),
  velocity(startVelocity),
  radarIsActive(false),
  sonarIsActive(false),
  subunits(),
  weapons(),
  linearTarget(linearTargetType_none),
  rotationalTarget(rotationalTargetType_none)
{
  initializeWeapons(
      universe->getUnitTypePtr(type), universe
    );
  hitPoints =
    universe->getUnitTypePtr(type)->getDynamicData().getMaxHitPoints();
}

void UnitStatus::initializeWeapons(
    const UnitType* typePtr,
    const Universe* universe
  )
{
  /* add weapons */
  const std::list<WeaponTypeID>& weaponTypes = typePtr->getWeapons();
  for (std::list<WeaponTypeID>::const_iterator weaponType = weaponTypes.begin();
      weaponType != weaponTypes.end(); weaponType++) {
    weapons.push_back(Weapon(universe->getWeaponTypePtr(*weaponType)));
  }
}

void UnitStatus::store(OArchive& out, const Universe* universe) const
{
  out << universe->getUnitTypePtr(type)->getInternalName();
  out << position;
  orientation.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive << subunits;
  /* TODO: weapons */
  out.insert<Order, orderCondition_max>(orders);
  currentOrder.store(out);
  out << uint8(linearTarget) << targetPosition <<
    targetVelocity << uint8(rotationalTarget);
  targetOrientation.store(out);
  out << targetAngularVelocity;
}

UnitStatus UnitStatus::load(IArchive& in, const Universe* universe)
{
  String typeName;
  UnitTypeID type;
  Point<sint32> position;
  Orientation orientation;
  Point<sint16> velocity;
  
  /* status stuff */
  HitPoints hitPoints;
  bool radarIsActive;
  bool sonarIsActive;
  std::list<uint32> subunits;
  std::list<Weapon> weapons;
  /* conditioned orders */
  Order orders[orderCondition_max];
  Order currentOrder;
  /* current goals which have been set by orders */
  LinearTargetType linearTarget;
  Point<sint32> targetPosition;
  Point<sint16> targetVelocity;
  RotationalTargetType rotationalTarget;
  Orientation targetOrientation;
  AngularVelocity targetAngularVelocity;
  
  in >> typeName;
  type = universe->getUnitTypeID(typeName);
  in >> position;
  orientation = Orientation::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive >> subunits;
  /* TODO: weapons */
  in.extract<Order, orderCondition_max>(orders);
  currentOrder = Order::load(in);
  in.extractEnum(linearTarget);
  in >> targetPosition >> targetVelocity;
  in.extractEnum(rotationalTarget);
  targetOrientation = Orientation::load(in);
  in >> targetAngularVelocity;

  return UnitStatus(
      type, position, orientation, velocity,
      hitPoints, radarIsActive, sonarIsActive, subunits,
      weapons, orders, currentOrder, linearTarget,
      targetPosition, targetVelocity,
      rotationalTarget, targetOrientation, targetAngularVelocity
    );
}

