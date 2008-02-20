#ifndef MATHSUTILS_H
#define MATHSUTILS_H

#include "libsakusen-global.h"

#include <boost/tuple/tuple.hpp>

#include "frame.h"

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

}

#endif // MATHSUTILS_H

