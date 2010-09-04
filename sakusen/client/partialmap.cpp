#include <sakusen/client/partialmap.h>
#include <sakusen/heightfield-methods.h>

using namespace sakusen::client;

void LIBSAKUSEN_CLIENT_API PartialMap::sanityCheck()
{
  if (0 != width() % heightfield.getXYResolution() ||
      0 != height() % heightfield.getXYResolution()) {
    SAKUSEN_FATAL(
        "heightfield resolution (" << heightfield.getXYResolution() <<
        ") not exact factor of map dimensions (" << width() << ", " <<
        height() << ")"
      );
  }
  if (heightfield.getWidth() !=
      1+width()/heightfield.getXYResolution()) {
    SAKUSEN_FATAL(
        "heightfield width (" << heightfield.getWidth() <<
        ") incorrect; it should be " <<
        1+width()/heightfield.getXYResolution()
      );
  }
  if (heightfield.getHeight() !=
      1+height()/heightfield.getXYResolution()) {
    SAKUSEN_FATAL(
        "heightfield height (" << heightfield.getHeight() <<
        ") incorrect; it should be " <<
        height()/heightfield.getXYResolution()+1
      );
  }
}

