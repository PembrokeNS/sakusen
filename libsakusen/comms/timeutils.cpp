#include "timeutils.h"

/** \brief Ensures that the given timeval is in canonical form
 *
 * This function manipulates the given timeval to ensure that tv.tv_usec lies
 * in the range [0, MICRO) */
void timeUtils_canonicalize(timeval& tv)
{
  while (tv.tv_usec >= MICRO) {
    tv.tv_usec -= MICRO;
    tv.tv_sec += 1;
  }
  while (tv.tv_usec < 0) {
    tv.tv_usec += MICRO;
    tv.tv_sec -= 1;
  }
}

