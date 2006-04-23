#include "update.h"

#include <ostream>

using namespace sakusen;

Update::Update() :
  data(NULL)
{
}

Update::Update(const Update& copy) :
  data( copy.data ? copy.data->newCopy() : NULL )
{
}

Update::Update(const UpdateData& d) :
  data( d.newCopy() )
{
}

Update::Update(UpdateData* d) :
  data(d)
{
}

Update::~Update()
{
  delete data;
  data = NULL;
}

Update& Update::operator=(const Update& copy)
{
  data = ( copy.data ? copy.data->newCopy() : NULL );
  return *this;
}

LIBSAKUSEN_API std::ostream& sakusen::operator<<(std::ostream& output, const Update& update)
{
  output << "Update { type=" << update.getType() << " }";
  return output;
}

Update Update::load(IArchive& in, const Universe* universe)
{
  /* If the archive is empty then it's broken.  Return a dud update */
  if (in.isFinished()) {
    return Update();
  }

  /* Get the type */
  UpdateType type;
  in.extractEnum(type);

  /* Construct the appropriate data */
  switch (type) {
    case updateType_unitRemoved:
      return Update(new UnitRemovedUpdateData(in));
    case updateType_unitAdded:
      return Update(new UnitAddedUpdateData(in, universe));
    case updateType_unitAltered:
      return Update(new UnitAlteredUpdateData(in, universe));
    case updateType_orderQueued:
      return Update(new OrderQueuedUpdateData(in));
    case updateType_orderCompleted:
      return Update(new OrderCompletedUpdateData(in));
    case updateType_orderAccepted:
      return Update(new OrderAcceptedUpdateData(in));
    case updateType_sensorReturnsRemoved:
      return Update(new SensorReturnsRemovedUpdateData(in));
    case updateType_sensorReturnsAdded:
      return Update(new SensorReturnsAddedUpdateData(in, universe));
    case updateType_sensorReturnsAltered:
      return Update(new SensorReturnsAlteredUpdateData(in, universe));
    default:
      Debug("Unknown UpdateType " << type);
      return Update();
  }
}

