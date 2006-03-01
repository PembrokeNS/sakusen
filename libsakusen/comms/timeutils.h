#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <sys/time.h>
#include <time.h>

#include "libsakusen-global.h"

#define MICRO 1000000

/** \file
 * This file defines some routines for manipulating time structures via
 * operator overloading
 */

void timeUtils_canonicalize(timeval& tv);

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

#endif // TIMEUTILS_H

