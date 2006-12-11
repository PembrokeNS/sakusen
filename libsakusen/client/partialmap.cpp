#include "partialmap.h"

using namespace sakusen::client;

void PartialMap::sanityCheck()
{
  if (0 != width() % heightfield.getHorizontalResolution() ||
      0 != height() % heightfield.getHorizontalResolution()) {
    Fatal(
        "heightfield resolution (" << heightfield.getHorizontalResolution() <<
        ", " << heightfield.getVerticalResolution() <<
        ") not eact factor of map dimensions (" << width() << ", " <<
        height() << ")"
      );
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getHorizontalResolution()) {
    Fatal("heightfield width incorrect");
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getVerticalResolution()) {
    Fatal(
        "heightfield height (" << heightfield.getHeight() <<
        ") incorrect; it should be " <<
        height()/heightfield.getVerticalResolution()+1
      );
  }
}

