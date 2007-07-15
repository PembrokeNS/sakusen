#ifndef CLIENTID_H
#define CLIENTID_H

#include "stringutils.h"
#include "idwrapper.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 */
class ClientId : public IdWrapper<uint8, ClientId> {
};

}

namespace __gnu_cxx {

template<>
struct hash<sakusen::ClientId> {
  private:
    hash<sakusen::ClientId::internal_type> intHasher;
  public:
    size_t operator()(const sakusen::ClientId i) const {
      return intHasher(i);
    }
};

}

#endif // CLIENTID_H

