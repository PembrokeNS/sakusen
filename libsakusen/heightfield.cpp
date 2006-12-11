#include "heightfield-methods.h"

#include "oarchive-methods.h"

using namespace std;
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
  if (UINT32_MAX / width < height) {
    Fatal("heightfield contains too much data");
  }
  /* Check that all possible height values fit into the proper range */
  if (verticalResolution == 0 || horizontalResolution == 0) {
    Fatal("heightfield resolution 0");
  }
  if (INT32_MAX / verticalResolution < INT16_MAX) {
    Fatal(
        "heights could overflow (vertical res is " << verticalResolution <<
        ")"
      );
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

  /* All this branching is not supposed to speed thing up, rather it is to
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
  sint32 result = INT32_MIN;
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

void Heightfield::store(OArchive& out) const
{
  out << horizontalResolution << verticalResolution << width << height;
  /** \todo Make neater */
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

