#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

namespace sakusen {

/** \brief Enumeration of types of thing that exist in the game world.
 *
 * This is a bitfield, so sets of types can be built with bitwise or.
 *
 * \warning Current serialization implementation limits us to 8 bits in this
 * bitfield.
 */
enum GameObject {
  gameObject_null          = 0x00,
  gameObject_land          = 0x01,
  gameObject_water         = 0x02,
  gameObject_unit          = 0x04,
  gameObject_sensorReturns = 0x08,
  gameObject_effect        = 0x10,
  gameObject_ballistic     = 0x20
};

inline GameObject operator|(const GameObject& left, const GameObject& right) {
  return static_cast<GameObject>(static_cast<int>(left) | right);
}

}

#endif // GAMEOBJECT_H

