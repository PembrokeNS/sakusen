#ifndef LIBSAKUSEN__ISENSORRETURNS_H
#define LIBSAKUSEN__ISENSORRETURNS_H

#include <sakusen/global.h>
#include <sakusen/gnu_extensions.h>
#include <sakusen/visibility.h>
#include <sakusen/playerid.h>
#include <sakusen/region.h>
#include <sakusen/rectangle.h>
#include <sakusen/sensorreturnsid.h>
#include <sakusen/ref.h>
#include <sakusen/world.h>
#include <sakusen/sensorreturnmap.h>

namespace sakusen {

/** \brief Interface to a collection of all sensor returns from a single unit
 * to a single player.
 *
 * \todo As well as an approximate location (the region), we need an
 * approximate size for the sensed unit. */
class LIBSAKUSEN_API ISensorReturns : public Bounded {
  public:
    virtual ~ISensorReturns() {}

    virtual SensorReturnsId getId() const = 0;
    virtual Perception getPerception() const = 0;
    virtual PlayerId getSenserOwner() const = 0;
    virtual PlayerId getSenseeOwner() const = 0;
    virtual Region<sint32>::ConstPtr getRegion() const = 0;
    virtual Ref<const ICompleteUnit> getUnit() const = 0;
    virtual const SensorReturnMap& getSensorReturns() const = 0;

    GameObject getObjectType() const { return gameObject_sensorReturns; }
    virtual Point<sint32> getBestPosition() const;
    virtual Rectangle<sint32> getBoundingRectangle() const;
    virtual Box<sint32> getBoundingBox() const;
    boost::tuple<double,double> intersect(const Ray& r) const;
    bool contains(const Position& p) const;
};

template<>
class SerializationHandler<ISensorReturns> {
  public:
    Ref<ISensorReturns> extract(
        IArchive& archive,
        const DeserializationContext& context
      ) const {
      SensorReturnsId id;
      archive >> id;
      return world->getISensorReturns(context.getPlayerId(), id);
    }
    void insert(OArchive& archive, const Ref<const ISensorReturns>& ret) const {
      archive << ret->getId();
    }
};

}

#endif // LIBSAKUSEN__ISENSORRETURNS_H

