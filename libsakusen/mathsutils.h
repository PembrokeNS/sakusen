#ifndef MATHSUTILS_H
#define MATHSUTILS_H

#include "libsakusen-global.h"

#include <boost/tuple/tuple.hpp>
#include <boost/algorithm/minmax.hpp>

namespace sakusen {

boost::tuple<double,double> LIBSAKUSEN_API mathsUtils_solveQuadratic(
    const double a,
    const double b,
    const double c
  );

}

#endif // MATHSUTILS_H

