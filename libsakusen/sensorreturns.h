#ifndef SENSORRETURNS_H
#define SENSORRETURNS_H

#include <list>

#include "isensorreturns.h"
#include "universe.h"
#include "completeunit.h"

namespace sakusen {

class SensorReturns : public ISensorReturns, private IRefContainer {
  private:
    SensorReturns();
    SensorReturns(
        SensorReturnsID i,
        Perception p,
        PlayerID srO,
        PlayerID seO,
        Region<sint32>* reg,
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
      region(NULL == copy->getRegion() ?
          NULL : new Region<sint32>(*copy->getRegion())),
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
      region(NULL == copy->getRegion() ?
          NULL : new Region<sint32>(*copy->getRegion())),
      unit(copy->getUnit().isValid() ?
          new CompleteUnit(copy->getUnit()) : NULL),
      returns(copy->getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
    SensorReturns(const SensorReturns& copy) :
      IReferent(copy),
      ISensorReturns(copy),
      IRefContainer(copy),
      id(copy.getId()),
      perception(copy.getPerception()),
      senserOwner(copy.getSenserOwner()),
      senseeOwner(copy.getSenseeOwner()),
      region(NULL == copy.getRegion() ?
          NULL : new Region<sint32>(*copy.getRegion())),
      unit(NULL == copy.unit ?
          NULL : new CompleteUnit(copy.getUnit())),
      returns(copy.getSensorReturns())
    {
      assert(0 == (perception & ~perception_full));
    }
    SensorReturns& operator=(const SensorReturns& copy)
    {
      /** \bug Doesn't handle self-assignment */
      ISensorReturns::operator=(copy);
      id = copy.getId();
      perception = copy.getPerception();
      assert(0 == (perception & ~perception_full));
      senserOwner = copy.getSenserOwner();
      senseeOwner = copy.getSenseeOwner();
      delete region;
      region = ( NULL == copy.getRegion() ?
          NULL : new Region<sint32>(*copy.getRegion()) );
      delete unit;
      unit = ( NULL == copy.unit ?
          NULL : new CompleteUnit(copy.getUnit()) );
      returns = copy.getSensorReturns();
      return *this;
    }
    ~SensorReturns()
    {
      delete region;
      region = NULL;
      while (!refs.empty()) {
        refs.back()->invalidate();
        refs.pop_back();
      }
      delete unit;
      unit = NULL;
    }
  private:
    SensorReturnsID id;
    Perception perception;
    PlayerID senserOwner;
    PlayerID senseeOwner;
    const Region<sint32>* region; /* owned by this */
    const CompleteUnit* unit; /* owned by this */
    mutable std::list<IRef*> refs;

    SensorReturnMap returns;

    void registerRef(IRef* ref) const {
      refs.push_back(ref);
    }
    void unregisterRef(IRef* ref) const {
      std::list<IRef*>::iterator it = find(refs.begin(), refs.end(), ref);
      if (it == refs.end()) {
        Fatal("Tried to unregister not-registered Ref");
      }
      refs.erase(it);
    }
  public:
    inline SensorReturnsID getId() const { return id; }
    inline Perception getPerception() const { return perception; }
    inline PlayerID getSenserOwner() const { return senserOwner; }
    inline PlayerID getSenseeOwner() const { return senseeOwner; }
    inline const Region<sint32>* getRegion() const { return region; }
    inline Ref<const ICompleteUnit> getUnit() const {
      return Ref<const ICompleteUnit>(unit, this);
    }
    inline const SensorReturnMap& getSensorReturns() const { return returns; }

    void store(OArchive&) const;
    static SensorReturns load(IArchive&, const Universe*);
};

}

#endif // SENSORRETURNS_H

