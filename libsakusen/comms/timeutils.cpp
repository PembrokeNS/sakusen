#include "timeutils.h"

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

