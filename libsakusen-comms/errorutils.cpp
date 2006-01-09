#include "errorutils.h"


#include <errno.h>

#include <sstream>
#include "libsakusen-comms-global.h"

#if defined (_MSC_VER)
#include <winsock2.h>
#include "wsabsd.h"
#endif //_MSC_VER

String errorUtils_parseErrno(int num)
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

String errorUtils_errorMessage(int num)
{
  /*char message[MESSAGE_BUFFER_LEN];*/
  /* HACK: This is the obsolete version of strerror_r - I can't find out how to
   * get at the non-obsolete version.  See strerror(3) for more details */
  /* This doesn't work anyway...
  if (NULL == strerror_r(num, message, MESSAGE_BUFFER_LEN)) {
    Fatal("error getting error message: " << errorUtils_parseErrno(errno));
  }
  return String(message); */
  /* Because the above doesn't work, we have FIXME: not thread-safe */
  return String(strerror(num));
}

