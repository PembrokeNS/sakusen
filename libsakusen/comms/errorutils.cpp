#include "errorutils.h"

#include <cerrno>
#include <sstream>
#include "libsakusen-comms-global.h"

#if defined (_MSC_VER)
#include <winsock2.h>
#include "wsabsd.h"
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif //_MSC_VER

String sakusen::comms::errorUtils_parseErrno(int num)
{
  switch(num) {
#define CASE(name) case name: return #name;
    CASE(EACCES)
    CASE(EAGAIN)
    CASE(EBADF)
    CASE(ECONNRESET)
    CASE(EDESTADDRREQ)
    CASE(EFAULT)
    CASE(EINTR)
    CASE(EINVAL)
    CASE(EISCONN)
    CASE(EMSGSIZE)
    CASE(ENOBUFS)
    CASE(ENOMEM)
    CASE(ENOTCONN)
    CASE(ENOTSOCK)
    CASE(EOPNOTSUPP)
    CASE(EPIPE)
    CASE(E2BIG)
    CASE(EILSEQ)
    CASE(EBADMSG)
#undef CASE
    default:
      std::ostringstream message;
      message << "UNKNOWN (" << num << ")";
      return message.str();
  }
}

#define MESSAGE_BUFFER_LEN 256

String sakusen::comms::errorUtils_errorMessage(int num)
{
#if defined (_MSC_VER)
    char message[MESSAGE_BUFFER_LEN];

    if (NULL == strerror_s(message, MESSAGE_BUFFER_LEN, num)) {
    Fatal("error getting error message: " << errorUtils_parseErrno(errno));
  }
  return String(message); 

#else

  /*char message[MESSAGE_BUFFER_LEN];*/
  /* HACK: This is the obsolete version of strerror_r - I can't find out how to
   * get at the non-obsolete version.  See strerror(3) for more details */
  /* This doesn't work anyway...
  if (NULL == strerror_r(num, message, MESSAGE_BUFFER_LEN)) {
    Fatal("error getting error message: " << errorUtils_parseErrno(errno));
  }
  return String(message); */
  /* Because the above doesn't work, we have FIXME: not thread-safe */
  /* \bug uses thread-unsafe strerror() */
  return String(strerror(num));
#endif //_MSC_VER
}

