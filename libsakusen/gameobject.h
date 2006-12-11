#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

namespace sakusen {

/** \brief Enumeration of things that exist in the game world */
enum GameObject {
  gameObject_land = 1,
  gameObject_water = 2,
  gameObject_unit = 4,
  gameObject_effect = 8
};

inline GameObject operator|(const GameObject& left, const GameObject& right) {
  return static_cast<GameObject>(static_cast<int>(left) | right);
}

}

#endif // GAMEOBJECT_H

