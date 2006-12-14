#ifndef ISENSORRETURNS_H
#define ISENSORRETURNS_H

#include "libsakusen-global.h"
#include "gnu_extensions.h"
#include "visibility.h"
#include "playerid.h"
#include "region.h"
#include "rectangle.h"
#include "sensorreturnsid.h"
#include "ref.h"
#include "world.h"

namespace sakusen {

typedef __gnu_cxx::hash_map<uint32, EachSensorReturn> SensorReturnMap;

/** \brief Interface to a collection of all sensor returns from a single unit
 * to a single player.
 *
 * \todo As well as an approximate location (the region), we need an
 * approximate size for the sensed unit. */
class LIBSAKUSEN_API ISensorReturns : virtual public IReferent {
  public:
    virtual ~ISensorReturns() {}

    virtual SensorReturnsID getId() const = 0;
    virtual Perception getPerception() const = 0;
    virtual PlayerID getSenserOwner() const = 0;
    virtual PlayerID getSenseeOwner() const = 0;
    virtual const Region<sint32>* getRegion() const = 0;
    virtual Ref<const ICompleteUnit> getUnit() const = 0;
    virtual const SensorReturnMap& getSensorReturns() const = 0;

    virtual Point<sint32> getBestPosition() const;
    virtual Rectangle<sint32> getBoundingRectangle() const;
};

template<>
class SerializationHandler<ISensorReturns> {
  public:
    Ref<ISensorReturns> extract(
        IArchive& archive,
        const PlayerID* player
      ) const {
      SensorReturnsID id;
      archive >> id;
      return world->getISensorReturns(*player, id);
    }
    void insert(OArchive& archive, const Ref<const ISensorReturns>& ret) const {
      archive << ret->getId();
    }
    typedef PlayerID loadArgument;
};

}

#endif // ISENSORRETURNS_H

