#include "update.h"

#include <ostream>

using namespace sakusen;

Update::Update(const UpdateData* d) :
  data(d)
{
}

LIBSAKUSEN_API std::ostream& sakusen::operator<<(std::ostream& output, const Update& update)
{
  output << "Update { type=" << update.getType() << " }";
  return output;
}

Update Update::load(
    IArchive& in,
    const DeserializationContext& context
  )
{
  /* If the archive is empty then it's broken.  Return a dud update */
  if (in.isFinished()) {
    throw EndOfArchiveDeserializationExn();
  }

  /* Get the type */
  UpdateType type;
  in.extractEnum(type);

  /* Construct the appropriate data */
  switch (type) {
    case updateType_unitRemoved:
      return Update(new UnitRemovedUpdateData(in));
    case updateType_unitAdded:
      return Update(new UnitAddedUpdateData(in, context));
    case updateType_unitAltered:
      return Update(new UnitAlteredUpdateData(in, context));
    case updateType_orderAccepted:
      return Update(new OrderAcceptedUpdateData(in, context));
    case updateType_sensorReturnsRemoved:
      return Update(new SensorReturnsRemovedUpdateData(in));
    case updateType_sensorReturnsAdded:
      return Update(new SensorReturnsAddedUpdateData(in, context));
    case updateType_sensorReturnsAltered:
      return Update(new SensorReturnsAlteredUpdateData(in, context));
    case updateType_ballisticRemoved:
      return Update(new BallisticRemovedUpdateData(in));
    case updateType_ballisticAdded:
      return Update(new BallisticAddedUpdateData(in));
    default:
      SAKUSEN_DEBUG("Unknown UpdateType " << type);
      throw EnumDeserializationExn("type", type);
  }
}

