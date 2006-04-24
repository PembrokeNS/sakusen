#include "visibility.h"

using namespace sakusen;

Visibility::Visibility() :
  optical(0),
  infraRed(0),
  radar(0),
  passiveRadar(0),
  sonar(0),
  passiveSonar(0),
  seismar(0)
{
}

Visibility::Visibility(IArchive& archive)
{
  archive >> optical >> infraRed >> radar >> passiveRadar >> sonar >>
    passiveSonar >> seismar;
}

void SensorCapability::updateReturn(
    SensorReturn& retrn,
    SensorType /*type*/,
    bool* changed,
    Perception* maxPerception,
    uint32* /*bestRadius*/
  ) const
{
  /** \todo Actually make this a real test, rather than an always-full result.
   * */
  if (retrn.perception != perception_unit) {
    retrn.perception = perception_unit;
    *changed = true;
  }
  *maxPerception |= perception_unit;
}

void Sensors::updateReturns(
    EachSensorReturn& returns,
    bool* changed,
    Perception* maxPerception,
    uint32* bestRadius
  ) const
{
  *maxPerception = perception_none;
#define UPDATE(type) \
  type.updateReturn( \
      returns.type, sakusen::type, changed, maxPerception, bestRadius \
    )
  UPDATE(optical);
  UPDATE(infraRed);
  UPDATE(seismar);
  UPDATE(radarPassive);
  UPDATE(radarActive);
  UPDATE(sonarPassive);
  UPDATE(sonarActive);
  UPDATE(seismar);
#undef UPDATE
}

