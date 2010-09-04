#include "frame.h"

namespace sakusen {

/** \brief Get the box in global coordinates bounding one in the Frame.
 *
 * The box whose bounds are computed is axis-aligned and centred on the origin
 * in the Frame coordinates, and extends in each direction awat from the origin
 * a distance specified by the \a size argument.
 */
Box<sint32> Frame::getBoundingBox(const Point<uint32>& size) const
{
  double dExtents[3];
  Point<sint32> pExtents;

  for (int i=0; i<3; i++) {
    dExtents[i] = 0;
    for (int j=0; j<3; j++) {
      dExtents[i] += fabs(orientation(i, j))*size[j];
    }
    pExtents[i] = sint32(ceil(dExtents[i]));
  }

  return Box<sint32>(position-pExtents, position+pExtents);
}

void Frame::store(OArchive& archive) const
{
  archive << position;
  orientation.store(archive);
}

Frame Frame::load(IArchive& archive)
{
  Position p;
  archive >> p;
  Orientation o = Orientation::load(archive);
  return Frame(p, o);
}

}

