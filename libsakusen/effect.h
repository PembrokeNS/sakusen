#ifndef EFFECT_H
#define EFFECT_H

class Effect;

#include "libsakusen-global.h"
#include "point.h"
#include "region.h"
#include "playerid.h"
#include "visibility.h"
#include "unit.h"

class Effect {
  private:
    PlayerID owner;
    Region region;
    Visibility visibility;
    /* TODO: some kind of visibility modification scheme goes here */

  protected:
    Effect() {}
    Effect(const Effect&) {}
    
  public:
    virtual ~Effect() {}
    /* accessors */
    inline const PlayerID getOwner(void) const { return owner; }
    inline const Region& getRegion(void) const { return region; }
    inline const Visibility& getVisibility(void) const { return visibility; }
    /* callbacks */
    virtual void onUnitPresent(Unit&) {}
    virtual void onUnitEnter(Unit&) {}
    virtual void onUnitLeave(Unit&) {}
    virtual void onDestruct(void) {}
};

#endif

