#ifndef EFFECT_H
#define EFFECT_H

namespace sakusen {
namespace server {

class Effect;

}}

#include "libsakusen-global.h"
#include "point.h"
#include "region.h"
#include "playerid.h"
#include "visibility.h"
#include "layeredunit.h"

namespace sakusen {
namespace server {

/** \brief Represents an alteration to some region of space in the game.
 *
 * \todo Some kind of visibility modification scheme. */
class Effect {
  private:
    PlayerID owner;
    Region<sint32> region;
    Visibility visibility;

    Effect();
  protected:
    Effect(const Effect& copy) :
      owner(copy.owner), region(copy.region), visibility(copy.visibility) {}
    Effect(PlayerID o, const Region<sint32>& r, Visibility v) :
      owner(o), region(r), visibility(v) {}
    
  public:
    virtual ~Effect() {}
    /* accessors */
    inline const PlayerID getOwner(void) const { return owner; }
    inline const Region<sint32>& getRegion(void) const { return region; }
    inline const Visibility& getVisibility(void) const { return visibility; }
    /* callbacks */
    virtual void onUnitPresent(Ref<LayeredUnit>&) {}
    virtual void onUnitEnter(Ref<LayeredUnit>&) {}
    virtual void onUnitLeave(Ref<LayeredUnit>&) {}
    /** \brief Test the effect for removal
     *
     * \return true iff the Effect should be removed from the World
     * 
     * Called every tick by CompleteWorld to allow the Effect to remove itself,
     * after the onUnitPresent calls.
     *
     * The default implementation returns true, so, unless overridden, the
     * Effect will last for but a single tick. */
    virtual bool onRemovalTest(void) { return true; }
};

}}

#endif

