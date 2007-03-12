#include "heightfield-methods.h"

#include "oarchive-methods.h"
#include "mathsutils.h"

using namespace std;
using namespace boost;
using namespace sakusen;

Heightfield::Heightfield(
    uint32 hR,
    uint32 vR,
    uint32 w,
    uint32 h,
    const hf_type& hf
  ) :
  horizontalResolution(hR),
  verticalResolution(vR),
  width(w),
  height(h),
  heightfield(hf)
{
  sanityCheck();
}

Heightfield::Heightfield(
    uint32 hR,
    uint32 vR,
    uint32 w,
    uint32 h
  ) :
  horizontalResolution(hR),
  verticalResolution(vR),
  width(w),
  height(h),
  heightfield(boost::extents[width][height])
{
  sanityCheck();
}

void Heightfield::sanityCheck() const
{
  /* Check that width*height fits into a uint32.  In theory we might be able to
   * support more on 64-bit computers with enough RAM, but it's best to keep it
   * sane for now. */
  if (width == 0 || height == 0) {
    Fatal("heightfield dimension 0");
  }
  if (numeric_limits<uint32>::max() / width < height) {
    Fatal("heightfield contains too much data");
  }
  /* Check that all possible height values fit into the proper range */
  if (verticalResolution == 0 || horizontalResolution == 0) {
    Fatal("heightfield resolution 0");
  }
  if (numeric_limits<sint32>::max() / verticalResolution <
      uint32(numeric_limits<sint16>::max())) {
    Fatal(
        "heights could overflow (vertical res is " << verticalResolution <<
        ")"
      );
  }
}

/** \brief Intersect ray with a particular cell of the heightfield
 *
 * This function asumes that there really is an intersection to be found
 */
double Heightfield::intersectRayInCell(
    const Ray& ray,
    double minimum,
    double extent,
    uint32 x,
    uint32 y
  ) const
{
  assert(x<width-1);
  assert(y<height-1);

  /* To do this properly we have to solve a hideously complex quadratic
   * equation, or maybe use some more prosaic numerical method.
   *
   * For now I'm trying to do it the quadratic equation way, but the
   * probability that I've got it right is slim.  Even if it is right, it's
   * probably horribly numerically unstable. */
  /** \todo Test this code */
  
  /* First we extract some heights */
  const double h00 = getHeightAtSample(x,   y);
  const double h01 = getHeightAtSample(x,   y+1);
  const double h10 = getHeightAtSample(x+1, y);
  const double h11 = getHeightAtSample(x+1, y+1);

  /* We also need to get the ray-origin in coordinates normalised to be [0,1]^2
   * in the cell */
  const double ox =
    double(ray.origin.x - world->getMap()->left())/horizontalResolution;
  const double oy =
    double(ray.origin.y - world->getMap()->bottom())/horizontalResolution;
  const double oz = double(ray.origin.z)/verticalResolution;

  /* And same for ray direction */
  const double dx = double(ray.d.x)/horizontalResolution;
  const double dy = double(ray.d.y)/horizontalResolution;
  const double dz = double(ray.d.z)/verticalResolution;

  /* Compute a useful difference */
  const double hdiff = h00 + h11 - h01 - h10;

  /* Compute the coefficients of the quadratic */
  const double a = dx*dy*hdiff;
  const double b = hdiff*(dx*oy + dy*ox) + dy*h01 + dx*h10 - (dy+dx)*h00 - dz;
  const double c = hdiff*ox*oy + oy*h01 + ox*h10 - (ox+oy)*h00 + h00 - oz;

  double r1, r2;

  tie(r1, r2) = mathsUtils_solveQuadratic(a, b, c);

  if (isnan(r1) || r2 < minimum) {
    /* Something's gone wrong... */
    Fatal("no valid solution found");
  }

  if (r1 >= minimum) {
    if (r1 <= extent) {
      return r1;
    } else {
      return std::numeric_limits<double>::infinity();
    }
  } else {
    if (r2 <= extent) {
      return r2;
    } else {
      return std::numeric_limits<double>::infinity();
    }
  }
}

sint32 Heightfield::getHeightAt(sint32 x, sint32 y) const
{
  uint32 xoff = x - world->getMap()->left();
  uint32 yoff = y - world->getMap()->bottom();

  /* We get the quotient and remainder when dividing each of xoff and yoff by
   * horizontalResolution */
  uint32 xq = xoff / horizontalResolution;
  uint32 xr = xoff % horizontalResolution;
  uint32 yq = yoff / horizontalResolution;
  uint32 yr = yoff % horizontalResolution;
  
  /** \bug There is a risk of overflow in various places
   * here.  Maybe we should cast up to sint64s */

  /* We do the interpolation in the x direction. */
  sint32 upperHeight;
  sint32 lowerHeight;

  /* All this branching is not supposed to speed things up, rather it is to
   * prevent accessing memory out of range in boundary cases */
  
  if (xr == 0) {
    lowerHeight = getHeightAtSample(xq, yq);
    if (yr == 0) {
      return lowerHeight;
    }
    upperHeight = getHeightAtSample(xq, yq+1);
  } else {
    lowerHeight = (xr * getHeightAtSample(xq+1, yq) +
        (horizontalResolution - xr) * getHeightAtSample(xq, yq)) /
      horizontalResolution;
    if (yr == 0) {
      return lowerHeight;
    }
    upperHeight = (xr * getHeightAtSample(xq+1, yq+1) +
        (horizontalResolution - xr) * getHeightAtSample(xq, yq+1)) /
      horizontalResolution;
  }
  /* And finally the interpolation in the y direction (the two are done
   * seperately for my sanity, and to reduce risk of overflow). */
  return (yr * upperHeight + (horizontalResolution - yr) * lowerHeight) /
    horizontalResolution;
}

sint32 Heightfield::getMaxHeightIn(const Rectangle<sint32>& area) const
{
  if (!area.intersects(world->getMap()->area())) {
    Fatal("rectangle outside map");
  }
  Rectangle<sint32> croppedArea = area.intersect(world->getMap()->area());
  sint32 result = numeric_limits<sint32>::min();
  /* We check the height at various positions, exploiting the fact that we know
   * the height is linearly interpolated between samples.
   *
   * Note that the efficiency of this depends on the fact that max is an inline
   * function rather than a macro. */

  uint32 minSampleX = dexToSampleFloorX(croppedArea.getMinX()) + 1;
  uint32 minSampleY = dexToSampleFloorY(croppedArea.getMinY()) + 1;
  uint32 maxSampleX = dexToSampleCeilX(croppedArea.getMaxX());
  uint32 maxSampleY = dexToSampleCeilY(croppedArea.getMaxY());
  /* The corners */
  result =
    max(result, getHeightAt(croppedArea.getMinX(), croppedArea.getMinY()));
  result =
    max(result, getHeightAt(croppedArea.getMaxX(), croppedArea.getMinY()));
  result =
    max(result, getHeightAt(croppedArea.getMinX(), croppedArea.getMaxY()));
  result =
    max(result, getHeightAt(croppedArea.getMaxX(), croppedArea.getMaxY()));
  /* The following edge computations are far from optimized */
  /* Across the top and bottom edges */
  for (uint32 x = minSampleX; x < maxSampleX; ++x) {
    result = max(result, getHeightAt(sampleToDexX(x), croppedArea.getMinY()));
    result = max(result, getHeightAt(sampleToDexX(x), croppedArea.getMaxY()));
  }
  /* Up the left and right edges */
  for (uint32 y = minSampleY; y < maxSampleY; ++y) {
    result = max(result, getHeightAt(croppedArea.getMinX(), sampleToDexY(y)));
    result = max(result, getHeightAt(croppedArea.getMaxX(), sampleToDexY(y)));
  }
  /* The internal points */
  for (uint32 x = minSampleX; x < maxSampleX; ++x) {
    for (uint32 y = minSampleY; y < maxSampleY; ++y) {
      result = max(result, getHeightAtSample(x, y));
    }
  }
  return result;
}

double Heightfield::intersectRay(const Ray& ray, double extent) const
{
  /* First we check to see whether it starts underground, in which case we
   * return 0 to indicate an instant hit */
  if (getHeightAt(ray.origin) > ray.origin.z) {
    return 0.0;
  }

  /* Use algorithm from
   *
   * A Fast Voxel Traversal Algorithm for Ray Tracing,
   * John Amanatides, Andrew Woo
   */
  const double inf = numeric_limits<double>::infinity();

  if (extent == inf) {
    /* This could well make us loop forever, so abort instead */
    Fatal("non-terminating intersection tests possible");
  }

  /* Locate starting heightfield sample cell */
  uint32 X = dexToSampleFloorX(ray.origin.x);
  uint32 Y = dexToSampleFloorY(ray.origin.y);
  sint32 stepX;
  sint32 stepY;
  double tMaxX;
  double tMaxY;
  double tDeltaX;
  double tDeltaY;
  /* Determine direction of stepping through cells, and find t-values at
   * which ray first crosses each of vertical and horizontal boundaries,
   * and find delta-t from one boundary to the next. */
  if (ray.d.x > 0) {
    stepX = 1;
    tMaxX = double(sampleToDexX(X+1)-ray.origin.x)/ray.d.x;
    tDeltaX = double(horizontalResolution)/ray.d.x;
  } else if (ray.d.x < 0) {
    stepX = -1;
    tMaxX = double(sampleToDexX(X)-ray.origin.x)/ray.d.x;
    tDeltaX = double(horizontalResolution)/-ray.d.x;
  } else {
    stepX = 0;
    tMaxX = inf;
    tDeltaX = inf;
  }

  if (ray.d.y > 0) {
    stepY = 1;
    tMaxY = double(sampleToDexY(Y+1)-ray.origin.y)/ray.d.y;
    tDeltaY = double(horizontalResolution)/ray.d.y;
  } else if (ray.d.y < 0) {
    stepY = -1;
    tMaxY = double(sampleToDexY(Y)-ray.origin.y)/ray.d.y;
    tDeltaY = double(horizontalResolution)/-ray.d.y;
  } else {
    stepY = 0;
    tMaxY = inf;
    tDeltaY = inf;
  }

  /* Ray position at which this cell was entered */
  double cellEntry = 0.0;

  /* loop through the cells */
  /** \bug We don't notice an intersection if the ray is above ground again by
   * the time it exits the cell */
  /** \bug We don't cope with reaching the boundaries */
  while (true) {
    if (tMaxX < tMaxY) {
      /* We're hitting an x-boundary before a y-boundary */
      Position p = ray.evaluate(tMaxX);
      if (p.z < getHeightAt(p)) {
        return intersectRayInCell(ray, cellEntry, extent, X, Y);
      }
      if (tMaxX > extent) {
        return inf;
      }
      X += stepX;
      cellEntry = tMaxX;
      tMaxX += tDeltaX;
    } else {
      /* We're hitting a y-boundary before an x-boundary */
      Position p = ray.evaluate(tMaxY);
      if (p.z < getHeightAt(p)) {
        return intersectRayInCell(ray, cellEntry, extent, X, Y);
      }
      if (tMaxY > extent) {
        return inf;
      }
      Y += stepY;
      cellEntry = tMaxY;
      tMaxY += tDeltaY;
    }
  }
}

void Heightfield::store(OArchive& out) const
{
  out << horizontalResolution << verticalResolution << width << height;
  out.insert<sint16, 2>(heightfield);
}

Heightfield Heightfield::load(IArchive& in)
{
  uint32 horizontalResolution;
  uint32 verticalResolution;
  uint32 width;
  uint32 height;
  boost::multi_array<sint16,2> heightfield;
  
  in >> horizontalResolution >> verticalResolution >> width >> height;
  in.extract<sint16,2>(heightfield);
  
  return Heightfield(
      horizontalResolution, verticalResolution, width, height, heightfield
    );
}

