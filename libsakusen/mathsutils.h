#ifndef MATHSUTILS_H
#define MATHSUTILS_H

#include "libsakusen-global.h"

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax.hpp>

namespace sakusen {

/** \brief Solves a quadratic and returns all available roots
 *
 * The parameters are \p a, \p b and \p c, where the quadratic to be solved is
 * ax^2+bx+c = 0, and we are solving for x.
 *
 * If a=b=0 then returns (NaN,NaN).
 * If a=0 and b!=0 then returns (x, NaN) where x is the root of the linear
 * equation.
 * If a!=0 and there is no root returns (NaN,Nan).
 * If a!=0 and there is a repeated root x returns (x,x).
 * If a!=0 and there are distinct roots x and y then returns (x,y) where x is
 * guaranteed to be less than y.
 */
inline boost::tuple<double,double> mathsUtils_solveQuadratic(
    const double a,
    const double b,
    const double c
  )
{
  /* I don't actually know what the differrence between a quiet and a signaling
   * NaN are, possibly this is the wrong sort */
  assert(std::numeric_limits<double>::has_quiet_NaN);
  const double nan = std::numeric_limits<double>::quiet_NaN();
  /* cope with the linear case */
  if (a==0) {
    if (b==0) {
      return boost::make_tuple(nan, nan);
    }
    return boost::make_tuple(-c/b, nan);
  }

  /* discriminant */
  const double d = b*b - 4*a*c;

  if (d < 0) {
    return boost::make_tuple(nan, nan);
  }

  /* Use some cunning quadratic solution method I found online */
  const double q = -0.5*(b + sgn(b)*sqrt(d));

  /* The roots are q/a and c/q. */
  return boost::minmax(c/q, q/a);
}

}

#endif // MATHSUTILS_H

