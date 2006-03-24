#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include "libsakusen-global.h"

#define MICRO 1000000

/** \file
 * This file defines some routines for manipulating time structures via
 * operator overloading
 */

void timeUtils_canonicalize(timeval& tv);

/** \brief Wrapper function for getting the time since it's OS-dependant */
inline void timeUtils_getTime(timeval* tv)
{
#ifdef WIN32
  /** \bug According to the docs, we should not use the system time to do
   * timeouts because it can change (e.g. when syncing with a timeserver).
   * However, that's exactly what we're doing. */
  SYSTEMTIME systemTime;
  GetSystemTime(&systemTime);
  FILETIME fileTime;
  SystemTimeToFileTime(&systemTime, &fileTime);
  ULARGE_INTEGER intTime;
  memcpy(&intTime, &fileTime, sizeof(ULARGE_INTEGER));
  /* Now intTime.QuadPart has the time in hundreds of nanoseconds since 1601 */

  /* The random-looking number is (I believe) the number
   * of seconds from the start of 1601 to the start of 1970
   * This has to be subtracted for the result to fit in an int32,
   * which a long may be. */
  tv->tv_sec = long(intTime.QuadPart / 10000000 - 11644732800);
  tv->tv_usec = long((intTime.QuadPart % 10000000) / 10);
#else
  gettimeofday(&endTime, NULL);
#endif
}

inline timeval operator+(timeval tv, sint32 usec)
{
  tv.tv_usec += usec;
  timeUtils_canonicalize(tv);
  return tv;
}

inline timeval& operator+=(timeval& tv, sint32 usec)
{
  tv.tv_usec += usec;
  timeUtils_canonicalize(tv);
  return tv;
}

inline timeval& operator+=(timeval& tv, const timeval& tu)
{
  tv.tv_usec += tu.tv_usec;
  tv.tv_sec += tu.tv_sec;
  timeUtils_canonicalize(tv);
  return tv;
}

inline sint32 operator-(const timeval& tv, const timeval& uv)
{
  return (tv.tv_sec - uv.tv_sec)*MICRO + tv.tv_usec - uv.tv_usec;
}

inline bool operator>(const timeval& tv, const timeval& uv)
{
  return timercmp(&tv, &uv, >);
}

inline bool operator<(const timeval& tv, const timeval& uv)
{
  return timercmp(&tv, &uv, <);
}

#endif // TIMEUTILS_H

