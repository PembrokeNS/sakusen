#ifndef CLIENTID_H
#define CLIENTID_H

#include "stringutils.h"
#include "idbase.h"

namespace sakusen {

/** \brief Typedef for unique client identifier, in case we ever want more than
 * 255 clients
 */
class ClientId : public IdBase<uint8, ClientId> {
};

}

#endif // CLIENTID_H

