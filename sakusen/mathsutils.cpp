#include "mathsutils.h"

#include "box.h"
#include "unitcorneriterator.h"

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
boost::tuple<double,double> LIBSAKUSEN_API mathsUtils_solveQuadratic(
    const double a,
    const double b,
    const double c
  )
{
  /* I don't actually know what the differrence between a quiet and a signaling
   * NaN is, possibly this is the wrong sort */
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

  if (b == 0) {
    return boost::make_tuple(-sqrt(-c/a), sqrt(-c/a));
  }

  /* Use some cunning quadratic solution method I found online */
  const double q = -0.5*(b + sgn(b)*sqrt(d));

  /* The roots are q/a and c/q. */
  return boost::minmax(c/q, q/a);
}

/** \brief Return true iff the given boxes intersect.
 *
 * Each box is defined by a frame of reference and a size; it is centred on the
 * origin and axis aligned in the frame of reference and the distance from the
 * origin along each axis is given by the size.
 *
 * To 'intersect' here means that one box contains either a corner or the
 * centre of the other, so there is plenty of space for false negatives.
 */
bool LIBSAKUSEN_API mathsUtils_boxesIntersect(
    const Frame& frame1, const Point<uint32>& size1,
    const Frame& frame2, const Point<uint32>& size2
  )
{
  const Box<sint32> box1(-Point<sint32>(size1), Point<sint32>(size1));
  const Box<sint32> box2(-Point<sint32>(size2), Point<sint32>(size2));

  if (box1.contains(frame1.globalToLocal(frame2.getPosition()))) {
    return true;
  }
  if (box2.contains(frame2.globalToLocal(frame1.getPosition()))) {
    return true;
  }

  for (UnitCornerIterator corner(frame2, size2), end; corner != end; ++corner) {
    if (box1.contains(frame1.globalToLocal(*corner))) {
      return true;
    }
  }
  for (UnitCornerIterator corner(frame1, size1), end; corner != end; ++corner) {
    if (box2.contains(frame2.globalToLocal(*corner))) {
      return true;
    }
  }
  return false;
}

}
