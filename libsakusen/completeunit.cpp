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

CompleteUnit::CompleteUnit(const Ref<const ICompleteUnit>& copy) :
  unitId(copy->getId()),
  status(copy->getIStatus()),
  typeData(copy->getITypeData())
{
}

/** \brief Serialize CompleteUnit
 *
 * \param sanitize If true, then the unitId will be zeroed in the serialized
 * version.  This is appropriate for when the CompleteUnit is being sent to a
 * client as part of a SensorReturns. */
void CompleteUnit::store(OArchive& out, bool sanitize) const
{
  out.magicValue("CUNIT");
  if (sanitize) {
    out << uint32(-1);
  } else {
    out << unitId;
  }
  status.store(out, world->getUniverse());
  typeData.store(out);
}

CompleteUnit CompleteUnit::load(
    IArchive& in,
    const Universe::ConstPtr* universe
  )
{
  uint32 unitId;
  
  in.magicValue("CUNIT");
  in >> unitId;
  UnitStatus status = UnitStatus::load(in, universe);
  UnitTypeData typeData = UnitTypeData::load(in);
  
  return CompleteUnit(
      unitId, status, typeData
    );
}

