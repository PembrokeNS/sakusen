#ifndef LIBSAKUSEN_COMMS__TIMEUTILS_H
#define LIBSAKUSEN_COMMS__TIMEUTILS_H

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

#include <sakusen/comms/global.h>

/** Number of microseconds in a second */
#define SAKUSEN_COMMS_TIMEUTILS_MICRO 1000000

/** \file
 * This file defines some routines for manipulating time structures via
 * operator overloading
 */

LIBSAKUSEN_COMMS_API void timeUtils_canonicalize(timeval& tv);

/** \brief Wrapper function for getting the time since it's OS-dependant */
LIBSAKUSEN_COMMS_API inline void timeUtils_getTime(timeval* tv)
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
  gettimeofday(tv, NULL);
#endif
}

/** \brief Wrapper for OS-depenant sleep functions */
LIBSAKUSEN_COMMS_API inline void timeUtils_sleep(const uint32& usec)
{
#ifdef WIN32
  Sleep(usec/1000);
#else
  usleep(usec);
#endif
}

/** \brief A Sleep function taking a timeval */
LIBSAKUSEN_COMMS_API inline void timeUtils_sleep(const timeval& tv)
{
  timeUtils_sleep(tv.tv_sec*SAKUSEN_COMMS_TIMEUTILS_MICRO+tv.tv_usec);
}

/** \brief Returns a timeval which is \a tv incremented by \a usec
 * microseconds */
LIBSAKUSEN_COMMS_API inline timeval operator+(timeval tv, sint32 usec)
{
  tv.tv_usec += usec;
  timeUtils_canonicalize(tv);
  return tv;
}

/** \brief Increases the timeval \a tv by \a usec
 * microseconds */
LIBSAKUSEN_COMMS_API inline timeval& operator+=(timeval& tv, sint32 usec)
{
  tv.tv_usec += usec;
  timeUtils_canonicalize(tv);
  return tv;
}

/** \brief Increases the timeval \a tv by \a tu */
LIBSAKUSEN_COMMS_API inline timeval& operator+=(timeval& tv, const timeval& uv)
{
  tv.tv_usec += uv.tv_usec;
  tv.tv_sec += uv.tv_sec;
  timeUtils_canonicalize(tv);
  return tv;
}

/** \brief Gets the number of microseconds from \a tv to \a tu */
LIBSAKUSEN_COMMS_API inline sint32 operator-(
    const timeval& tv,
    const timeval& uv
  )
{
  return (tv.tv_sec - uv.tv_sec)*SAKUSEN_COMMS_TIMEUTILS_MICRO +
    tv.tv_usec - uv.tv_usec;
}

/** \brief Compares the two timeval arguments */
LIBSAKUSEN_COMMS_API inline bool operator>(const timeval& tv, const timeval& uv)
{
  return timercmp(&tv, &uv, >);
}

/** \brief Compares the two timeval arguments */
LIBSAKUSEN_COMMS_API inline bool operator<(const timeval& tv, const timeval& uv)
{
  return timercmp(&tv, &uv, <);
}

#endif // LIBSAKUSEN_COMMS__TIMEUTILS_H

