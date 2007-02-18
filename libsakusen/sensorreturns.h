#ifndef SENSORRETURNS_H
#define SENSORRETURNS_H

#include <list>

#include "isensorreturns.h"
#include "universe.h"
#include "completeunit.h"

namespace sakusen {

class SensorReturns : public ISensorReturns {
  private:
    SensorReturns(
        SensorReturnsID i,
        Perception p,
        PlayerID srO,
        PlayerID seO,
        Region<sint32>::Ptr reg,
        CompleteUnit* s,
        SensorReturnMap ret
      ) :
      id(i),
      perception(p),
      senserOwner(srO),
      senseeOwner(seO),
      region(reg),
      unit(s),
      returns(ret)
    {}
  public:
    SensorReturns(const ISensorReturns* copy) :
      ISensorReturns(*copy),
      id(copy->getId()),
      perception(copy->getPerception()),
      senserOwner(copy->getSenserOwner()),
      senseeOwner(copy->getSenseeOwner()),
      region(copy->getRegion()),
      unit(copy->getUnit().isValid() ?
          new CompleteUnit(copy->getUnit()) : NULL),
      returns(copy->getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
    SensorReturns(const Ref<const ISensorReturns>& copy) :
      ISensorReturns(),
      id(copy->getId()),
      perception(copy->getPerception()),
      senserOwner(copy->getSenserOwner()),
      senseeOwner(copy->getSenseeOwner()),
      region(copy->getRegion()),
      unit(copy->getUnit().isValid() ?
          new CompleteUnit(copy->getUnit()) : NULL),
      returns(copy->getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
  private:
    SensorReturnsID id;
    Perception perception;
    PlayerID senserOwner;
    PlayerID senseeOwner;
    Region<sint32>::ConstPtr region;
    CompleteUnit::ConstPtr unit;

    SensorReturnMap returns;
  public:
    inline SensorReturnsID getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    inline PlayerID getSenserOwner() const { return senserOwner; }
    inline PlayerID getSenseeOwner() const { return senseeOwner; }
    inline Region<sint32>::ConstPtr getRegion() const { return region; }
    inline Ref<const ICompleteUnit> getUnit() const {
      return Ref<const ICompleteUnit>(unit);
    }
    inline const SensorReturnMap& getSensorReturns() const { return returns; }

    void store(OArchive&) const;
    static SensorReturns load(IArchive&, const Universe::ConstPtr*);
};

}

#endif // SENSORRETURNS_H

