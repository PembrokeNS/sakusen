#ifndef LIBSAKUSEN__HITPOINTALTERATION_H
#define LIBSAKUSEN__HITPOINTALTERATION_H

namespace sakusen {

/** \brief Enumeration of types of change which happen to a unit's hit points
 * when its max hit points change */
enum hitPointAlteration {
  /** Hit points are set to the new maximum */
  fullHitPoints,
  /** Hit points are kept at the same fraction of the maximum (up to rounding
   * errors) */
  scaleHitPoints,
  /** Hit points are kept at a fixed value (but truncated if higher than the
   * new maximum) */
  fixHitPoints
};

}

#endif // LIBSAKUSEN__HITPOINTALTERATION_H

