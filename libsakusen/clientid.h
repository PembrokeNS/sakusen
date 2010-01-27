#ifndef LIBSAKUSEN__CLIENTID_H
#define LIBSAKUSEN__CLIENTID_H

#include "idbase.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 */
class ClientId : public IdBase<uint8, ClientId> {
};

}

#endif // LIBSAKUSEN__CLIENTID_H

