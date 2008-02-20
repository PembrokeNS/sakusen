#include "frame.h"

namespace sakusen {

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

