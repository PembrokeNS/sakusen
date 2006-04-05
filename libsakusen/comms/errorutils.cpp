#include "errorutils.h"

#include <cerrno>
#include <sstream>
#include "libsakusen-comms-global.h"

String sakusen::comms::errorUtils_parseErrno(int num)
{
  switch(num) {
#define CASE(name) case name: return #name;
    CASE(EACCES)
    CASE(EAGAIN)
#if EAGAIN != EWOULDBLOCK
    CASE(EWOULDBLOCK)
#endif
    CASE(EBADF)
    CASE(ECONNRESET)
    CASE(ECONNABORTED)
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
    CASE(ENOENT)
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
    return "error getting error message for errno " + errorUtils_parseErrno(num);
  }
  return String(message); 

#else

  /*char message[MESSAGE_BUFFER_LEN];*/
  /* HACK: This is the obsolete version of strerror_r - I can't find out how to
   * get at the non-obsolete version.  See strerror(3) for more details */
  /* This doesn't work anyway...
  if (NULL == strerror_r(num, message, MESSAGE_BUFFER_LEN)) {
    return "error getting error message for errno " + errorUtils_parseErrno(errno);
  }
  return String(message); */
  /* Because the above doesn't work, we have FIXME: not thread-safe */
  /* \bug uses thread-unsafe strerror() */
  return String(strerror(num));
#endif //_MSC_VER
}

