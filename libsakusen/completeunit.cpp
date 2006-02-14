#include "completeunit.h"

#include "unit.h"
#include "oarchive-methods.h"

using namespace std;

using namespace sakusen;

CompleteUnit::CompleteUnit(
    UnitTypeID t,
    uint32 uI,
    const Point<sint32>& p,
    const Orientation& o,
    const Point<sint16>& v,
    HitPoints mHP,
    uint8 m,
    const Point<uint32>& s,
    const Region<sint16>& pA,
    const Region<sint16>& pV,
    const Region<sint16>& pAV,
    const Visibility& visib,
    const Sensors& visio,
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
  unitId(uI),
  position(p),
  orientation(o),
  velocity(v),
  maxHitPoints(mHP),
  mass(m),
  size(s),
  possibleAccelerations(pA),
  possibleVelocities(pV),
  possibleAngularVelocities(pAV),
  visibility(visib),
  vision(visio),
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
  memcpy(orders, ord, sizeof(orders));
}

CompleteUnit::CompleteUnit(const IUnit* copy) :
  type(copy->getType()),
  unitId(copy->getId()),
  position(copy->getPosition()),
  orientation(copy->getOrientation()),
  velocity(copy->getVelocity()),
  maxHitPoints(copy->getMaxHitPoints()),
  mass(copy->getMass()),
  size(copy->getSize()),
  possibleAccelerations(copy->getPossibleAccelerations()),
  possibleVelocities(copy->getPossibleVelocities()),
  possibleAngularVelocities(copy->getPossibleAngularVelocities()),
  visibility(copy->getVisibility()),
  vision(copy->getVision()),
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

void CompleteUnit::store(OArchive& out) const
{
  out.magicValue<5>("CUNIT");
  out << getTypePtr()->getInternalName();
  out << maxHitPoints;
  out << mass;
  out << size;
  possibleAccelerations.store(out);
  possibleVelocities.store(out);
  possibleAngularVelocities.store(out);
  visibility.store(out);
  vision.store(out);
  out.magicValue<8>("CUNIT-ST");
  out << unitId << position;
  orientation.store(out);
  out << velocity << hitPoints << radarIsActive << sonarIsActive << subunits;
  /* TODO: weapons */
  out.magicValue<8>("CUNIT-OR");
  out.insert<Order, orderCondition_max>(orders);
  currentOrder.store(out);
  out.magicValue<8>("CUNIT-TA");
  out << uint8(linearTarget) << targetPosition <<
    targetVelocity << uint8(rotationalTarget);
  targetOrientation.store(out);
  out << targetAngularVelocity;
}

CompleteUnit CompleteUnit::load(IArchive& in, const Universe* universe)
{
  String typeName;
  UnitTypeID type;
  uint32 unitId;
  Point<sint32> position;
  Orientation orientation;
  Point<sint16> velocity;
  
  /* modifiable stuff from UnitType */
  HitPoints maxHitPoints;
  uint8 mass;
  Point<uint32> size;
  Region<sint16> possibleAccelerations;
  Region<sint16> possibleVelocities;
  Region<sint16> possibleAngularVelocities;
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
  
  in.magicValue<5>("CUNIT");
  in >> typeName;
  type = universe->getUnitTypeID(typeName);
  in >> maxHitPoints;
  in >> mass;
  in >> size;
  possibleAccelerations = Region<sint16>::load(in);
  possibleVelocities = Region<sint16>::load(in);
  possibleAngularVelocities = Region<sint16>::load(in);
  Visibility visibility(in);
  Sensors vision(in);
  in.magicValue<8>("CUNIT-ST");
  in >> unitId >> position;
  orientation = Orientation::load(in);
  in >> velocity >> hitPoints >> radarIsActive >> sonarIsActive >> subunits;
  /* TODO: weapons */
  in.magicValue<8>("CUNIT-OR");
  //in >> orders;
  in.extract<Order, orderCondition_max>(orders);
  currentOrder = Order::load(in);
  in.magicValue<8>("CUNIT-TA");
  in.extractEnum(linearTarget);
  in >> targetPosition >> targetVelocity;
  in.extractEnum(rotationalTarget);
  targetOrientation = Orientation::load(in);
  in >> targetAngularVelocity;
  return CompleteUnit(
      type, unitId, position, orientation, velocity, maxHitPoints, mass, size,
      possibleAccelerations, possibleVelocities, possibleAngularVelocities,
      visibility, vision, hitPoints, radarIsActive, sonarIsActive, subunits,
      weapons, orders, currentOrder, linearTarget,
      targetPosition, targetVelocity,
      rotationalTarget, targetOrientation, targetAngularVelocity
    );
}

