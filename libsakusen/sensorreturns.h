#ifndef SENSORRETURNS_H
#define SENSORRETURNS_H

#include <list>

#include "isensorreturns.h"
#include "universe.h"
#include "completeunit.h"

namespace sakusen {

/** \brief Encapsulates conglomerated information from all sensor returns from
 * one unit to one player.
 *
 * This is a bare implementation of ISensorReturns, which no extra
 * functionality.
 */
class LIBSAKUSEN_API SensorReturns : public ISensorReturns {
  private:
    SensorReturns(
        SensorReturnsId i,
        Perception p,
        PlayerId srO,
        PlayerId seO,
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
    SensorReturnsId id;
    Perception perception;
    PlayerId senserOwner;
    PlayerId senseeOwner;
    Region<sint32>::ConstPtr region;
    CompleteUnit::ConstPtr unit;

    SensorReturnMap returns;
  public:
    inline SensorReturnsId getId() const { return id; }
    /** \brief Get information acquired by sensor returns
     *
     * Returns a bitfield of the various things that this SensorReturns can
     * tell about the sensee.
     */
    inline Perception getPerception() const { return perception; }
    /** \brief Get owner of this SensorReturns. */
    inline PlayerId getSenserOwner() const { return senserOwner; }
    /** \brief Get sensee's owner's id
     *
     * If the sensee is known, then returns its id.  If not, then return 0
     */
    inline PlayerId getSenseeOwner() const { return senseeOwner; }
    /** \brief Get region indicating approximate location of sensee
     *
     * If the SensorReturns carries an approximate region (which it will in
     * exactly those cases where it doesn't have an entire copy of the unit)
     * then return a pointer to it.  Otherwise return null pointer.
     */
    inline Region<sint32>::ConstPtr getRegion() const { return region; }
    /** \brief Get unit info of sensee
     *
     * If the SensorReturns carries an entire unit then return a Ref to it.
     * Otherwise return null Ref.
     */
    inline Ref<const ICompleteUnit> getUnit() const {
      return Ref<const ICompleteUnit>(unit);
    }
    inline const SensorReturnMap& getSensorReturns() const { return returns; }

    void store(OArchive&) const;
    static SensorReturns load(IArchive&, const DeserializationContext&);
};

/** \brief Function object to extract the id from an SensorReturns */ 
struct SensorReturnsIder {
  SensorReturnsId operator()(const Ref<SensorReturns>& s) const {
    return s->getId();
  }
};

}

#endif // SENSORRETURNS_H

