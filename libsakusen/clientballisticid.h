#ifndef CLIENTBALLISTICID_H
#define CLIENTBALLISTICID_H

#include "idbase.h"

namespace sakusen {

/** \brief Id for a particular player's perspective of a ballictic.
 *
 * Each player that can see a Ballistic will give it an id.  These are
 * independent of one another; there is no overall id for a Ballistic, and if
 * no one can see it then it has no id at all. */
class ClientBallisticId : public IdBase<uint32, ClientBallisticId> {
};

}

namespace __gnu_cxx {

template<>
struct hash<sakusen::ClientBallisticId> {
  private:
    hash<sakusen::ClientBallisticId::internal_type> intHasher;
  public:
    size_t operator()(const sakusen::ClientBallisticId i) const {
      return intHasher(i);
    }
};

}

#endif // CLIENTBALLISTICID_H

