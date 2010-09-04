#ifndef LIBSAKUSEN_SERVER__DYNAMICSENSORRETURNS_H
#define LIBSAKUSEN_SERVER__DYNAMICSENSORRETURNS_H

#include <sakusen/libsakusen-global.h>
#include <sakusen/hash_list.h>
#include <sakusen/visibility.h>
#include <sakusen/isensorreturns.h>
#include <sakusen/serializationhandler.h>

namespace sakusen {
namespace server {

class Player;
class LayeredUnit;

class DynamicSensorReturns : public ISensorReturns {
  private:
    DynamicSensorReturns();
  public:
    DynamicSensorReturns(
        SensorReturnsId id,
        const Ref<const LayeredUnit>& sensee,
        Player* senserOwner
      );
    ~DynamicSensorReturns() {}
  private:
    SensorReturnsId id;
    Perception perception;
    Region<sint32>::ConstPtr region;
    Player* senserOwner;
    SensorReturnMap sensers;
    Ref<const LayeredUnit> sensee;
    /** \brief When set, indicates that there have been changes to this that
     * have yet to be sent to the sensorOwner's clients. */
    bool dirty;
  public:
    /** \name Accessors. */
    //@{
    inline SensorReturnsId getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    PlayerId getSenserOwner() const;
    PlayerId getSenseeOwner() const;
    Region<sint32>::ConstPtr getRegion() const;
    Ref<const ICompleteUnit> getUnit() const;
    inline const SensorReturnMap& getSensorReturns() const { return sensers; }
    inline bool empty() const { return sensers.empty(); }
    //@}
    inline bool isDirty() const { return dirty; }
    inline void setDirty() { dirty = true; }
    inline void clearDirty() { dirty = false; }
    void senseeDestroyed();
    void update();
};

/** \brief Function object to extract the id from an SensorReturns */
struct DynamicSensorReturnsIder {
  SensorReturnsId operator()(const Ref<DynamicSensorReturns>& s) const {
    return s->getId();
  }
};

/** \brief Typedef for an iterator type stored often to refer to a
 * DynamicSensorReturns.
 *
 * It is this type because this is an iterator into the container stored by a
 * Player.  The typedef exists because the type name is so long.
 *
 * \todo I beleive that this is not in fact necessary any more, and could be
 * replaced everywhere by a Ref<DynamicSensorReturns.  It may
 * conceivably be faster to stick with this method, but that may not be reason
 * enough for the complication. */
typedef hash_list<DynamicSensorReturns, DynamicSensorReturns>::iterator
  DynamicSensorReturnsRef;

}}

#endif // LIBSAKUSEN_SERVER__DYNAMICSENSORRETURNS_H

