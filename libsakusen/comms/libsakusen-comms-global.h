#ifndef LIBSAKUSEN_COMMS_GLOBAL_H
#define LIBSAKUSEN_COMMS_GLOBAL_H

#include "libsakusen-global.h"

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
#if 0 //Make this one a static library also.
  #ifdef LIBSAKUSEN_COMMS_EXPORTS
    #define LIBSAKUSEN_COMMS_API __declspec(dllexport)
  #else
    #define LIBSAKUSEN_COMMS_API __declspec(dllimport)
  #endif //LIBSAKUSEN_COMMS_EXPORTS
#else 
	#define LIBSAKUSEN_COMMS_API
#endif //if 0, static library.

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

#ifdef WIN32
  #define socket_errno WSAGetLastError()
#else
  #define socket_errno errno
#endif

/* The value 108 is used in sys/un.h, but not declared to anything.  The
 * unix(7) man page uses the name UNIX_PATH_MAX, so I have followed suit */
#define UNIX_PATH_MAX 108

/* NETWORK_PROTOCOL_VERSION is the version number of the sakusen network
 * protocol.  It must fit in a uint8.  It should remain at 0 until at least
 * v1.0 of some server and client exist.  If we hit version ~100, we should
 * think about expanding it to two bytes. */
#define NETWORK_PROTOCOL_VERSION 0

/* BUFFER_LEN is the max length allowed for any sakusen message sent over the
 * wire */
#define BUFFER_LEN 4096

/** ADDR_DELIM is the delimiter for sakusen-style socket addresses */
#define ADDR_DELIM "|"

/* SOCKET_SUBDIR is the subdirectory of CONFG_SUBDIR where servers are
 * expected to put their sockets */
#define SOCKET_SUBDIR FILE_SEP "servers"

/* SETTINGS_DELIMITER is the character used to split the names of nodes when
 * constructing full names of nodes in the settings tree */
#define SETTINGS_DELIMITER ":"

/** DEFAULT_PORT is the port that sockets default to using if none is specified
 * */
#define DEFAULT_PORT 1723

#endif // LIBSAKUSEN_COMMS_GLOBAL_H

