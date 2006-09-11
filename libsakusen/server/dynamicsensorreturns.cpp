#include "dynamicsensorreturns.h"

#include "player.h"
#include "layeredunit.h"
#include "sphereregiondata.h"
#include "limits.h"

using namespace __gnu_cxx;

using namespace sakusen;
using namespace sakusen::server;

DynamicSensorReturns::DynamicSensorReturns(
    SensorReturnsID i,
    const LayeredUnit* s,
    Player* sO
  ) :
  id(i),
  perception(perception_none),
  region(NULL),
  senserOwner(sO),
  sensers(1),
  sensee(s),
  dirty(false)
{
  update();
}

PlayerID DynamicSensorReturns::getSenserOwner() const
{
  return senserOwner->getId();
}

PlayerID DynamicSensorReturns::getSenseeOwner() const
{
  if (0 != (perception & (perception_owner | perception_unit))) {
    return sensee->getOwner();
  } else {
    return 0;
  }
}

const Region<sint32>* DynamicSensorReturns::getRegion() const
{
  if (0 != (perception & perception_region)) {
    return region;
  } else {
    return NULL;
  }
}

const ICompleteUnit* DynamicSensorReturns::getUnit() const
{
  if (0 != (perception & perception_unit)) {
    return sensee;
  } else {
    return NULL;
  }
}

/** \brief Update the sensor return for a new game state */
void DynamicSensorReturns::update()
{
  /* If the player is player 0 then they get to see everything (so that
   * observers can see everything) */
  if (senserOwner->getId() == 0) {
    if (perception != perception_unit) {
      perception = perception_unit;
      dirty = true;
    }
    return;
  }
    
  /* If the player owns the unit, then clear and quit */
  if (sensee->getOwner() == senserOwner->getId()) {
    sensers.clear();
    return;
  }
  
  perception = perception_none;
  uint32 bestRadius = UINT32_MAX;
  
  /* loop over all units of the player doing the sensing */
  /** \todo Loop only over the nearby units
   * \todo Stop once it is clear that adding more units will add no more
   * information. */
  EachSensorReturn workingCopy;
  
  for (hash_map<uint32, LayeredUnit*>::const_iterator unitIt =
      senserOwner->getUnits().begin(); unitIt != senserOwner->getUnits().end();
      ++unitIt) {
    /* seek out local unit/sensors corresponding to this unit */
    SensorReturnMap::iterator returnsIt = sensers.find(unitIt->first);
    EachSensorReturn* returns;
    if (returnsIt == sensers.end()) {
      workingCopy.clear();
      returns = &workingCopy;
    } else {
      returns = &returnsIt->second;
    }
    Perception thisPerception;
    unitIt->second->getITypeData()->getVision().updateReturns(
        *returns, &dirty, &thisPerception, &bestRadius
      );
    if (returnsIt != sensers.end() && thisPerception == perception_none) {
      sensers.erase(returnsIt);
    } else if (returnsIt == sensers.end() &&
        thisPerception != perception_none) {
      sensers[unitIt->first] = *returns;
    }
    perception |= thisPerception;
  }

  if (0 != (perception & perception_region)) {
    /* We need to set dirty every time, because presumably the random component
     * could change every tick, and also we aren't informed by the unit
     * whenever its position changes, so that could have altered too. */
    dirty = true;
    delete region;
    /* \todo Add some randomness to the position of the centre of the region,
     * otherwise it might as well just be a point */
    region = new Region<sint32>(new SphereRegionData<sint32>(
          sensee->getIStatus()->getPosition(), bestRadius
        ));
  } else {
    delete region;
    region = NULL;
  }
}

