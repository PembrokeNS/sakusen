#include "completeunit.h"

#include "oarchive-methods.h"
#include "world.h"

using namespace std;

using namespace sakusen;

CompleteUnit::CompleteUnit(
    uint32 uI,
    const UnitStatus& st,
    const UnitTypeData& tD
  ) :
  unitId(uI),
  status(st),
  typeData(tD)
{
}

CompleteUnit::CompleteUnit(const ICompleteUnit* copy) :
  unitId(copy->getId()),
  status(copy->getIStatus()),
  typeData(copy->getITypeData())
{
}

void CompleteUnit::store(OArchive& out) const
{
  out.magicValue<5>("CUNIT");
  out << unitId;
  status.store(out, world->getUniverse());
  typeData.store(out);
}

CompleteUnit CompleteUnit::load(IArchive& in, const Universe* universe)
{
  uint32 unitId;
  
  in.magicValue<5>("CUNIT");
  in >> unitId;
  UnitStatus status = UnitStatus::load(in, universe);
  UnitTypeData typeData = UnitTypeData::load(in);
  
  return CompleteUnit(
      unitId, status, typeData
    );
}

