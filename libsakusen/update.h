#ifndef UPDATE_H
#define UPDATE_H

#include "updatedata.h"
#include "updatetype.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

class LIBSAKUSEN_API Update {
  private:
    Update();
  public:
    Update(IArchive& in, const Universe*);
    Update(const Update& copy);
    Update(const UpdateData& data);
    ~Update();
    Update& operator=(const Update& copy);
  private:
    const UpdateData* data;
  public:
    inline UpdateType getType(void) const { return data->getType(); }
    inline void store(OArchive& out) const {
      out << uint8(getType());
      data->store(out);
    }
    inline UnitRemovedUpdateData getUnitRemovedData(void) const {
      return *dynamic_cast<const UnitRemovedUpdateData*>(data);
    }
    inline UnitAddedUpdateData getUnitAddedData(void) const {
      return *dynamic_cast<const UnitAddedUpdateData*>(data);
    }
    inline OrderAcceptedUpdateData getOrderAcceptedData(void) const {
      return *dynamic_cast<const OrderAcceptedUpdateData*>(data);
    }
    inline OrderCompletedUpdateData getOrderCompletedData(void) const {
      return *dynamic_cast<const OrderCompletedUpdateData*>(data);
    }
    inline OrderQueuedUpdateData getOrderQueuedData(void) const {
      return *dynamic_cast<const OrderQueuedUpdateData*>(data);
    }
};

std::ostream& operator<<(std::ostream& output, const Update& update);

}

#endif // UPDATE_H

