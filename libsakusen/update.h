#ifndef UPDATE_H
#define UPDATE_H

#include "updatedata.h"
#include "updatetype.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

class LIBSAKUSEN_API Update {
  public:
    /** \brief Constructs Update around given data
     *
     * Calling this transfers ownership of \p data to this */
    explicit Update(const UpdateData* data);
  private:
    UpdateData::ConstPtr data;
  public:
    /** \brief Get the type of this Update
     *
     * Primarily intended for (de)serialization
     */
    inline UpdateType getType(void) const { return data->getType(); }
#define GET(type) \
    inline type##UpdateData get##type##Data(void) const { \
      return dynamic_cast<const type##UpdateData&>(*data); \
    }
    GET(UnitRemoved)
    GET(UnitAdded)
    GET(UnitAltered)
    GET(OrderAccepted)
    GET(SensorReturnsRemoved)
    GET(SensorReturnsAdded)
    GET(SensorReturnsAltered)
    GET(BallisticRemoved)
    GET(BallisticAdded)
#undef GET

    typedef Universe::ConstPtr loadArgument;
    typedef PlayerID loadArgument2;
    inline void store(OArchive& out) const {
      out.insertEnum(getType());
      data->store(out);
    }
    static Update load(IArchive& in, const loadArgument*, const loadArgument2*);
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, const Update& update);

}

#endif // UPDATE_H

