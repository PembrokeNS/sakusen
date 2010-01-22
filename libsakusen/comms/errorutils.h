#ifndef LIBSAKUSEN_COMMS__ERRORUTILS_H
#define LIBSAKUSEN_COMMS__ERRORUTILS_H

#include "libsakusen-global.h"
#include "libsakusen-comms-global.h"

#if defined (_MSC_VER)
#include <winsock2.h>
#include "wsabsd.h"
#endif //_MSC_VER

namespace sakusen {
namespace comms {

/** \brief Turn a numeric errno into its string name (e.g. "EACCES") */
LIBSAKUSEN_COMMS_API String errorUtils_parseErrno(int num);

/** \brief Get an appropriate error message for an errno */
LIBSAKUSEN_COMMS_API String errorUtils_errorMessage(int num);

}}

#endif // LIBSAKUSEN_COMMS__ERRORUTILS_H

