#include "completeunit.h"

#include "oarchive-methods.h"
#include "world.h"

using namespace std;

using namespace sakusen;

CompleteUnit::CompleteUnit(
    uint32 uI,
    const UnitStatus& st,
    HitPoints mHP,
    uint8 m,
    const Point<uint32>& s,
    const Region<sint16>& pA,
    const Region<sint16>& pV,
    const Region<sint16>& pAV,
    const Visibility& visib,
    const Sensors& visio
  ) :
  unitId(uI),
  status(st),
  maxHitPoints(mHP),
  mass(m),
  size(s),
  possibleAccelerations(pA),
  possibleVelocities(pV),
  possibleAngularVelocities(pAV),
  visibility(visib),
  vision(visio)
{
}

CompleteUnit::CompleteUnit(const ICompleteUnit* copy) :
  unitId(copy->getId()),
  status(copy->getStatus()),
  maxHitPoints(copy->getMaxHitPoints()),
  mass(copy->getMass()),
  size(copy->getSize()),
  possibleAccelerations(copy->getPossibleAccelerations()),
  possibleVelocities(copy->getPossibleVelocities()),
  possibleAngularVelocities(copy->getPossibleAngularVelocities()),
  visibility(copy->getVisibility()),
  vision(copy->getVision())
{
}

void CompleteUnit::store(OArchive& out) const
{
  out.magicValue<5>("CUNIT");
  out << unitId;
  status.store(out, world->getUniverse());
  out << maxHitPoints;
  out << mass;
  out << size;
  possibleAccelerations.store(out);
  possibleVelocities.store(out);
  possibleAngularVelocities.store(out);
  visibility.store(out);
  vision.store(out);
}

CompleteUnit CompleteUnit::load(IArchive& in, const Universe* universe)
{
  uint32 unitId;
  
  /* modifiable stuff from UnitType */
  HitPoints maxHitPoints;
  uint8 mass;
  Point<uint32> size;
  
  in.magicValue<5>("CUNIT");
  in >> unitId;
  UnitStatus status = UnitStatus::load(in, universe);
  in >> maxHitPoints;
  in >> mass;
  in >> size;
  Region<sint16> possibleAccelerations = Region<sint16>::load(in);
  Region<sint16> possibleVelocities = Region<sint16>::load(in);
  Region<sint16> possibleAngularVelocities = Region<sint16>::load(in);
  Visibility visibility(in);
  Sensors vision(in);
  
  return CompleteUnit(
      unitId, status, maxHitPoints, mass, size,
      possibleAccelerations, possibleVelocities, possibleAngularVelocities,
      visibility, vision
    );
}

