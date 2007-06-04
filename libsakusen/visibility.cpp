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

/** \brief Update the given SensorReturn for this SensorCapability for one type
 * of sensor.
 *
 * \param[in,out] retrn         SensorReturn to update.
 * \param[in]     type          Type of sensor used.
 * \param[in,out] changed       Set to true iff anything changes.  Not set to
 *                              false if nothing changes.
 * \param[in,out] maxPerception Updated by or-ing in all perceptions this
 *                              sensor achieves.
 * \param[in,out] bestRadius    Reduced if this sensor can detect to within a
 *                              smaller radius than the value presently
 *                              indicates.
 */
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

/** \brief Update the given EachSensorReturn for this Sensors for all types
 * of sensor.
 *
 * \param[in,out] returns    EachSensorReturn to update.
 * \param[in,out] changed    Set to true iff anything changes.  Not set to false
 *                          if nothing changes.
 * \param[in,out] maxPerception Updated by or-ing in all perceptions these
 *                          Sensors achieve.
 * \param[in,out] bestRadius Reduced if these sensors can detect to within a
 *                          smaller radius than the value presently indicates.
 */
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

