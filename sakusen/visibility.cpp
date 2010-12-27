#include <sakusen/visibility.h>

#include <sakusen/completeunit.h>

using namespace sakusen;

Visibility::Visibility() :
  optical(Visibility::default_),
  infraRed(Visibility::default_),
  radarPassive(Visibility::default_),
  radarActive(Visibility::default_),
  sonarPassive(Visibility::default_),
  sonarActive(Visibility::default_),
  seismar(Visibility::default_)
{
}

Visibility::Visibility(IArchive& archive)
{
  archive >> optical >> infraRed >> radarPassive >> radarActive >>
    sonarPassive >> sonarActive >> seismar;
}

/* A table of (the floors of) the first 16 non-negative integer powers of
 * 2^(1/16), scaled up by a factor of 2^15 so as to lie in the range
 * [2^15,2^16) */
static const uint32 powerLookup[16] = {
  0x8000,
  0x85AA,
  0x8B95,
  0x91C3,
  0x9837,
  0x9EF5,
  0xA5FE,
  0xAD58,
  0xB504,
  0xBD08,
  0xC567,
  0xCE24,
  0xD744,
  0xE0CC,
  0xEAC0,
  0xF525
};

/** \brief Update the given SensorReturn for this SensorCapability for one type
 * of sensor.
 *
 * \param[in]     senser        Unit doing the sensing.
 * \param[in]     sensee        Unit possibly being sensed.
 * \param[in,out] retrn         SensorReturn to update.
 * \param[in]     type          Type of sensor used.
 * \param[in,out] changed       Set to true iff anything changes.  Not set to
 *                              false if nothing changes.
 * \param[in,out] maxPerception Updated by or-ing in all perceptions this
 *                              sensor achieves.
 * \param[in,out] bestRadius    Reduced if this sensor can detect to within a
 *                              smaller radius than the value presently
 *                              indicates.
 *
 * It's inline in the hope that when it's
 * used in Sensors::updateReturns below, the
 * value of 'type' can be evaluated at compile-time and avoid several branches.
 * */
inline void SensorCapability::updateReturn(
    const Ref<const ICompleteUnit>& senser,
    const Ref<const ICompleteUnit>& sensee,
    SensorReturn& retrn,
    SensorType type,
    bool* changed,
    Perception* maxPerception,
    uint32* bestRadius
  ) const
{
  if (!capable) {
    return;
  }
  const IUnitStatus& senserStatus = senser->getIStatus();
  if (type == radarActive && !senserStatus.isRadarActive()) {
    return;
  }
  if (type == sonarActive && !senserStatus.isSonarActive()) {
    return;
  }

  const IUnitTypeData& senseeTypeData = sensee->getITypeData();
  const Visibility& senseeVisibility = senseeTypeData.getVisibility();
  Visibility::type visibility = senseeVisibility.get(type);
  
  if (visibility == Visibility::max) {
    if (retrn.perception != perception_unit) {
      retrn.perception = perception_unit;
      *changed = true;
    }
    *maxPerception |= perception_unit;
    return;
  }

  if (range == 0 || visibility == Visibility::min) {
    return;
  }

  if (range == boost::integer_traits<uint32>::const_max) {
    if (retrn.perception != perception_unit) {
      retrn.perception = perception_unit;
      *changed = true;
    }
    *maxPerception |= perception_unit;
    return;
  }

  /* That concludes the special-case checks; now we proceed with the more
   * normalish stuff */

  const IUnitStatus& senseeStatus = sensee->getIStatus();

  /** \todo Take into account line of sight, effects, randomness, which should
   * all either cause an abrupt return or adjust the variable visibility
   * (probably by adding/subtracting something).
   * */

  const uint32 distanceModifier =
    (1 << (visibility >> 4)) * powerLookup[visibility & 0xf];
  /* Now rangeModifier is a value in the range [2^15, 2^32-1) with the
   * 'default' value (from visibility middle-of-the-range) being 2^23 */
  const Point<sint32> displacement =
    senseeStatus.getFrame().getPosition()-senserStatus.getFrame().getPosition();
  const uint64 effectiveDistance =
    (uint64(displacement.length()) << 23) / distanceModifier;
  /* Use integer division so we can only change behaviour at integer multiples
   * of range */
  const uint64 normalizedDistance = effectiveDistance / range;
  switch (normalizedDistance) {
    case 0:
      if (retrn.perception != perception_unit) {
        retrn.perception = perception_unit;
        *changed = true;
      }
      *maxPerception |= perception_unit;
      return;
    case 1:
      {
        if (retrn.perception != perception_region) {
          retrn.perception = perception_region;
          *changed = true;
        }
        /** \todo Think of a vaguely sensible formula */
        const uint32 thisRadius = effectiveDistance - range;
        if (thisRadius < *bestRadius) {
          *bestRadius = thisRadius;
          *changed = true;
        }
        *maxPerception |= perception_region;
      }
      return;
    default:
      /* See nothing */
      return;
  }
}

/** \brief Update the given EachSensorReturn for this Sensors for all types
 * of sensor.
 *
 * \param[in]     senser        Unit doing the sensing.
 * \param[in]     sensee        Unit possibly being sensed.
 * \param[in,out] returns       EachSensorReturn to update.
 * \param[in,out] changed       Set to true iff anything changes.  Not set to
 *                              false if nothing changes.
 * \param[in,out] maxPerception Updated by or-ing in all perceptions these
 *                              Sensors achieve.
 * \param[in,out] bestRadius    Reduced if these sensors can detect to within a
 *                              smaller radius than the value presently
 *                              indicates.
 */
void Sensors::updateReturns(
    const Ref<const ICompleteUnit>& senser,
    const Ref<const ICompleteUnit>& sensee,
    EachSensorReturn& returns,
    bool* changed,
    Perception* maxPerception,
    uint32* bestRadius
  ) const
{
  *maxPerception = perception_none;
#define UPDATE(type) \
  type.updateReturn( \
      senser, sensee, returns.type, sakusen::type, changed, maxPerception, \
      bestRadius \
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

