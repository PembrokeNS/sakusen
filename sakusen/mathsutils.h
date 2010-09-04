#ifndef LIBSAKUSEN__MATHSUTILS_H
#define LIBSAKUSEN__MATHSUTILS_H

#include <sakusen/global.h>

#include <boost/tuple/tuple.hpp>

#include <sakusen/frame.h>

namespace sakusen {

boost::tuple<double,double> LIBSAKUSEN_API mathsUtils_solveQuadratic(
    const double a,
    const double b,
    const double c
  );

bool LIBSAKUSEN_API mathsUtils_boxesIntersect(
    const Frame& frame1, const Point<uint32>& size1,
    const Frame& frame2, const Point<uint32>& size2
  );

template<typename T>
inline LIBSAKUSEN_API T mathsUtils_divideRoundUp(T const n, T const d)
{
  return (n+d-1)/d;
}

}

#endif // LIBSAKUSEN__MATHSUTILS_H

