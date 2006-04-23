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
    Update(const Update& copy);
    Update(const UpdateData& data);
    Update(UpdateData* data);
    ~Update();
    Update& operator=(const Update& copy);
  private:
    const UpdateData* data;
  public:
    inline UpdateType getType(void) const { return data->getType(); }
#define GET(type) \
    inline type##UpdateData get##type##Data(void) const { \
      return *dynamic_cast<const type##UpdateData*>(data); \
    }
    GET(UnitRemoved)
    GET(UnitAdded)
    GET(UnitAltered)
    GET(OrderAccepted)
    GET(OrderCompleted)
    GET(OrderQueued)
    GET(SensorReturnsRemoved)
    GET(SensorReturnsAdded)
    GET(SensorReturnsAltered)
#undef GET

    typedef Universe loadArgument;
    inline void store(OArchive& out) const {
      out.insertEnum(getType());
      data->store(out);
    }
    static Update load(IArchive& in, const loadArgument*);
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, const Update& update);

}

#endif // UPDATE_H

