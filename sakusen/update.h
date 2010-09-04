#ifndef LIBSAKUSEN__UPDATE_H
#define LIBSAKUSEN__UPDATE_H

#include <sakusen/updatedata.h>
#include <sakusen/updatetype.h>
#include <sakusen/iarchive.h>
#include <sakusen/oarchive.h>

namespace sakusen {

class LIBSAKUSEN_API Update {
  public:
    /** \brief Constructs Update around given data
     *
     * Calling this transfers ownership of \p data to this */
    explicit Update(const UpdateData* data);

    /** \brief Fake constructor to make python bindings happy
     * */
    inline Update() { throw new std::runtime_error("Called the fake constructor"); }
  private:
    UpdateData::ConstPtr data;
  public:
    /** \brief Get the type of this Update
     *
     * Primarily intended for (de)serialization
     */
    inline UpdateType getType(void) const { return data->getType(); }
#define SAKUSEN_UPDATE_GET(type) \
    inline type##UpdateData get##type##Data(void) const { \
      return dynamic_cast<const type##UpdateData&>(*data); \
    }
    SAKUSEN_UPDATE_GET(UnitRemoved)
    SAKUSEN_UPDATE_GET(UnitAdded)
    SAKUSEN_UPDATE_GET(UnitAltered)
    SAKUSEN_UPDATE_GET(OrderAccepted)
    SAKUSEN_UPDATE_GET(SensorReturnsRemoved)
    SAKUSEN_UPDATE_GET(SensorReturnsAdded)
    SAKUSEN_UPDATE_GET(SensorReturnsAltered)
    SAKUSEN_UPDATE_GET(BallisticRemoved)
    SAKUSEN_UPDATE_GET(BallisticAdded)
#undef SAKUSEN_UPDATE_GET

    inline void store(OArchive& out) const {
      out.insertEnum(getType());
      data->store(out);
    }
    static Update load(IArchive&, const DeserializationContext&);
};

LIBSAKUSEN_API std::ostream& operator<<(std::ostream& output, const Update& update);

}

#endif // LIBSAKUSEN__UPDATE_H

