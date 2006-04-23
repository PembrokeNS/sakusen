#ifndef SENSORRETURNS_H
#define SENSORRETURNS_H

#include "isensorreturns.h"
#include "universe.h"
#include "completeunit.h"

namespace sakusen {

class SensorReturns : public ISensorReturns {
  private:
    SensorReturns();
    SensorReturns(
        SensorReturnsID i,
        Perception p,
        PlayerID o,
        Region<sint32>* reg,
        CompleteUnit* s,
        SensorReturnMap ret
      ) :
      id(i),
      perception(p),
      owner(o),
      region(reg),
      unit(s),
      returns(ret)
    {}
  public:
    SensorReturns(const ISensorReturns* copy) :
      ISensorReturns(*copy),
      id(copy->getId()),
      perception(copy->getPerception()),
      owner(copy->getOwner()),
      region(NULL == copy->getRegion() ?
          NULL : new Region<sint32>(*copy->getRegion())),
      unit(NULL == copy->getUnit() ?
          NULL : new CompleteUnit(copy->getUnit())),
      returns(copy->getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
    SensorReturns(const SensorReturns& copy) :
      ISensorReturns(copy),
      id(copy.getId()),
      perception(copy.getPerception()),
      owner(copy.getOwner()),
      region(NULL == copy.getRegion() ?
          NULL : new Region<sint32>(*copy.getRegion())),
      unit(NULL == copy.getUnit() ?
          NULL : new CompleteUnit(copy.getUnit())),
      returns(copy.getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
    SensorReturns& operator=(const SensorReturns& copy)
    {
      ISensorReturns::operator=(copy);
      id = copy.getId();
      perception = copy.getPerception();
      assert(0 == (perception & ~perception_full));
      owner = copy.getOwner();
      delete region;
      region = ( NULL == copy.getRegion() ?
          NULL : new Region<sint32>(*copy.getRegion()) );
      delete unit;
      unit = ( NULL == copy.getUnit() ?
          NULL : new CompleteUnit(copy.getUnit()) );
      returns = copy.getSensorReturns();
      return *this;
    }
    ~SensorReturns()
    {
      delete region;
      region = NULL;
      delete unit;
      unit = NULL;
    }
  private:
    SensorReturnsID id;
    Perception perception;
    PlayerID owner;
    const Region<sint32>* region; /* owned by this */
    const CompleteUnit* unit; /* owned by this */
    SensorReturnMap returns;
  public:
    inline SensorReturnsID getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    inline PlayerID getOwner() const { return owner; }
    inline const Region<sint32>* getRegion() const { return region; }
    inline const ICompleteUnit* getUnit() const { return unit; }
    inline const SensorReturnMap& getSensorReturns() const { return returns; }

    void store(OArchive&) const;
    static SensorReturns load(IArchive&, const Universe*);
};

}

#endif // SENSORRETURNS_H

