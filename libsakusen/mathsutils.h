#ifndef MATHSUTILS_H
#define MATHSUTILS_H

#include "libsakusen-global.h"

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax.hpp>

namespace sakusen {

inline boost::tuple<double,double> mathsUtils_solveQuadratic(
    const double a,
    const double b,
    const double c
  )
{
  /* cope with the linear case */
  if (a==0) {
    return -c/b;
  }

  /* discriminant */
  const double d = b*b - 4*a*c;

  /* I don't actually know what the differrence between a quiet and a signaling
   * NaN are, possibly this is the wrong sort */
  assert(std::numeric_limits<double>::has_quiet_NaN);
  if (d < 0) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    return boost::make_tuple(nan, nan);
  }

  /* Use some cunning quadratic solution method I found online */
  const double q = -0.5*(b + sgn(b)*sqrt(d));

  /* The roots are q/a and c/q. */
  return boost::minmax(c/q, q/a);
}

}

#endif // MATHSUTILS_H

