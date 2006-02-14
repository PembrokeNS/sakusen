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

class Effect {
  private:
    PlayerID owner;
    Region<sint32> region;
    Visibility visibility;
    /* TODO: some kind of visibility modification scheme goes here */

  protected:
    Effect() {}
    Effect(const Effect&) {}
    
  public:
    virtual ~Effect() {}
    /* accessors */
    inline const PlayerID getOwner(void) const { return owner; }
    inline const Region<sint32>& getRegion(void) const { return region; }
    inline const Visibility& getVisibility(void) const { return visibility; }
    /* callbacks */
    virtual void onUnitPresent(LayeredUnit&) {}
    virtual void onUnitEnter(LayeredUnit&) {}
    virtual void onUnitLeave(LayeredUnit&) {}
    virtual void onDestruct(void) {}
};

}}

#endif

