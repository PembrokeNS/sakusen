#include "partialmap.h"

using namespace sakusen::client;

void PartialMap::sanityCheck()
{
  if (0 != width() % heightfield.getXYResolution() ||
      0 != height() % heightfield.getXYResolution()) {
    Fatal(
        "heightfield resolution (" << heightfield.getXYResolution() <<
        ") not exact factor of map dimensions (" << width() << ", " <<
        height() << ")"
      );
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getXYResolution()) {
    Fatal("heightfield width incorrect");
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getXYResolution()) {
    Fatal(
        "heightfield height (" << heightfield.getHeight() <<
        ") incorrect; it should be " <<
        height()/heightfield.getXYResolution()+1
      );
  }
}

