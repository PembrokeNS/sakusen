#ifndef LIBSAKUSEN_COMMS_GLOBAL_H
#define LIBSAKUSEN_COMMS_GLOBAL_H

#include <sakusen/global.h>

#include <cerrno>

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_COMMS_EXPORTS
    #define LIBSAKUSEN_COMMS_API __declspec(dllexport)
  #else
    #define LIBSAKUSEN_COMMS_API __declspec(dllimport)
  #endif //LIBSAKUSEN_COMMS_EXPORTS

  //Uses the bounds checked versions of certain string functions
  #undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
  #define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#else
  #if defined(__GNUC__) && (__GNUC__ >= 4)
    #define LIBSAKUSEN_COMMS_API __attribute__ ((visibility ("default")))
  #else
    #define LIBSAKUSEN_COMMS_API
  #endif
#endif

namespace sakusen { namespace comms {
#ifdef WIN32
  inline int socketErrno() { return WSAGetLastError(); }
#else
  inline int socketErrno() { return errno; }
#endif
}}

/** The value 108 is used in sys/un.h on linux/glibc, but not declared to
 * anything.  The
 * unix(7) man page uses the name UNIX_PATH_MAX, so we have followed suit.
 * \bug The value is 104 on FreeBSD and quite possibly 0 on the Hurd.
 * Therefore, we should use a better way. See Debian bug #187391 */
#define SAKUSEN_COMMS_UNIX_PATH_MAX 104

/* The version number of the sakusen network
 * protocol.  It must fit in a uint8.  It should remain at 0 until at least
 * v1.0 of some server and client exist.  If we hit version ~100, we should
 * think about expanding it to two bytes. */
#define SAKUSEN_COMMS_NETWORK_PROTOCOL_VERSION 0

/** The max length allowed for any sakusen message sent over the
 * wire.
 *
 * \bug This is surely unnecessary. */
#define SAKUSEN_COMMS_BUFFER_LEN 1000000

/** The delimiter for sakusen-style socket addresses */
#define SAKUSEN_COMMS_ADDR_DELIM "|"

/** The subdirectory of SAKUSEN_CONFIG_SUBDIR where servers
 * are expected to put their sockets */
#define SAKUSEN_COMMS_SOCKET_SUBDIR "servers"

/** The character used to split the names of nodes when
 * constructing full names of nodes in the settings tree */
#define SAKUSEN_COMMS_SETTINGS_DELIMITER ":"

/** The port that sockets default to using if none is specified */
#define SAKUSEN_COMMS_DEFAULT_PORT 15498

#endif // LIBSAKUSEN_COMMS_GLOBAL_H

