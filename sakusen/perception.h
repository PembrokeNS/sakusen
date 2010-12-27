#ifndef LIBSAKUSEN__PERCEPTION_H
#define LIBSAKUSEN__PERCEPTION_H

namespace sakusen {

/** \brief Describes a degree of perception
 *
 * A bit-field enumeration describing to what extent a unit is perceived.  Each
 * bit correponds to a piece of information about the unit.  When that bit is
 * set, that information is perceived; when the bit is not set, it is not.
 *
 * \todo Perhaps allow for finer gradations of revelation. */
enum Perception {
  perception_none = 0, /**< No information is perceived */
  perception_region = 1, /**< An approximate region is known */
  perception_owner = 2, /**< The owner of the unit known */
  perception_unit = 4, /**< The entire unit is known */
  perception_full = 7 /**< All possible information is known.  This value
                        should be the | of all the others */
};

inline Perception& operator|=(Perception& left, const Perception& right) {
  left = static_cast<Perception>(left | right);
  return left;
}

}

#endif // LIBSAKUSEN__PERCEPTION_H

