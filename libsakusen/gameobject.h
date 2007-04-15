#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

namespace sakusen {

/** \brief Enumeration of things that exist in the game world */
enum GameObject {
  gameObject_land =          0x01,
  gameObject_water =         0x02,
  gameObject_unit =          0x04,
  gameObject_sensorReturns = 0x08,
  gameObject_effect =        0x10
};

inline GameObject operator|(const GameObject& left, const GameObject& right) {
  return static_cast<GameObject>(static_cast<int>(left) | right);
}

}

#endif // GAMEOBJECT_H

