#include "icompleteunit.h"

using namespace sakusen;

/** \brief Compute and return the bounding rectangle for this unit.
 *
 * \note This method is relatively slow, so it is worth storing the bounding
 * rectangle if you need it more than once and know the unit has not moved
 * between times.
 */
Rectangle<sint32> ICompleteUnit::getBoundingRectangle(void) const
{
  double dExtents[2];
  uint32 iExtents[2];
  const Frame& frame = getIStatus().getFrame();
  const Point<sint32>& pos = frame.getPosition();
  const Orientation& orientation = frame.getOrientation();
  const Point<uint32>& size = getITypeData().getSize();
  
  for (int i=0; i<2; i++) {
    dExtents[i] = 0;
    for (int j=0; j<3; j++) {
      dExtents[i] += fabs(orientation(i, j))*size[j];
    }
    iExtents[i] = uint32(ceil(dExtents[i]));
  }

  return Rectangle<sint32>(
      pos.x-iExtents[0], pos.y-iExtents[1], pos.x+iExtents[0], pos.y+iExtents[1]
  );
}

Box<sint32> ICompleteUnit::getBoundingBox(void) const
{
  double dExtents[3];
  Point<sint32> pExtents;
  const Frame& frame = getIStatus().getFrame();
  const Point<sint32>& pos = frame.getPosition();
  const Orientation& orientation = frame.getOrientation();
  const Point<uint32>& size = getITypeData().getSize();
  
  for (int i=0; i<3; i++) {
    dExtents[i] = 0;
    for (int j=0; j<3; j++) {
      dExtents[i] += fabs(orientation(i, j))*size[j];
    }
    pExtents[i] = sint32(ceil(dExtents[i]));
  }

  return Box<sint32>(pos-pExtents, pos+pExtents);
}

boost::tuple<double,double> ICompleteUnit::intersect(const Ray& r) const
{
  const Frame& frame = getIStatus().getFrame();
  const Point<sint32> size(getITypeData().getSize());

  /* We transform the ray into local coordinates */
  Ray localRay(
      frame.globalToLocal(r.origin),
      frame.globalToLocalRelative(r.d)
    );
  /* Now we can intersect this transformed ray with our box */
  return localRay.intersectBox(Box<sint32>(-size, size));
}

bool ICompleteUnit::contains(const Position& p) const
{
  const Frame& frame = getIStatus().getFrame();
  const Point<sint32> size(getITypeData().getSize());

  /* We transform the position into local coordinates */
  Position localPos(frame.globalToLocal(p));
  /* Now we can check this transformed point against our box */
  return Box<sint32>(-size, size).contains(localPos);
}

