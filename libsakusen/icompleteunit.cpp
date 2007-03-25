#include "icompleteunit.h"

using namespace sakusen;

Rectangle<sint32> ICompleteUnit::getBoundingRectangle(void) const
{
  double dExtents[2];
  uint32 iExtents[2];
  const Point<sint32>& pos = getIStatus()->getPosition();
  const Orientation& orientation = getIStatus()->getOrientation();
  const Point<uint32>& size = getITypeData()->getSize();
  
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
  const Point<sint32>& pos = getIStatus()->getPosition();
  const Orientation& orientation = getIStatus()->getOrientation();
  const Point<uint32>& size = getITypeData()->getSize();
  
  for (int i=0; i<3; i++) {
    dExtents[i] = 0;
    for (int j=0; j<3; j++) {
      dExtents[i] += fabs(orientation(i, j))*size[j];
    }
    pExtents[i] = sint32(ceil(dExtents[i]));
  }

  return Box<sint32>(pos-pExtents, pos+pExtents);
}

double ICompleteUnit::intersect(const Ray& r) const
{
  const IUnitStatus* status = getIStatus();
  const Point<uint32>& size = getITypeData()->getSize();

  /* We transform the ray into local coordinates */
  Ray localRay(
      status->globalToLocal(r.origin),
      status->globalToLocalRelative(r.d)
    );
  /* Now we can intersect this transformed ray with our box */
  return localRay.intersectBox(Box<sint32>(-size, size));
}

bool ICompleteUnit::contains(const Position& p) const
{
  const IUnitStatus* status = getIStatus();
  const Point<uint32>& size = getITypeData()->getSize();

  /* We transform the position into local coordinates */
  Position localPos(status->globalToLocal(p));
  /* Now we can check this transformed point against our box */
  return Box<sint32>(-size, size).contains(localPos);
}

