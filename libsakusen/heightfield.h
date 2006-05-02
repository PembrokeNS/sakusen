#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#include "libsakusen-global.h"
#include "point.h"
#include "rectangle.h"
#include "iarchive.h"
#include "oarchive.h"

namespace sakusen {

/** \brief Stores heightfield data
 *
 * This class stores height data in a simple array and provides methods for
 * computing the height at a given point on the map.  Heightfield data can be
 * at a coarser resolution than the finest resolution the game understands,
 * both horizontally and vertically (independantly).  Heights are linearly
 * interpolated between samples.
 *
 * \todo Do we need separate x and y resolutions?
 *
 * The array of heights is one unit wider and taller than might at first be
 * expected, because we need to know the heights at all four edges of the map,
 * so as to be able to interpolate.  Beware of fencepost errors.  Also, on
 * topologies where some edges are identified, it is the responsibility of the
 * author of the heightfield data to ensure that the values match up properly,
 * to prevent discontinuities in the heightfield at map edges.
 *
 * At present, the serialization of heightfields is simple copying of the whole
 * array.  In future we need to be able to store heightfields as images for
 * compression and sanity.  For the moment this is not done, because it
 * requires extra libraries and similar messiness.
 *
 * \todo It might be worthwhile storing the map left and bottom values here
 * rather than looking them up whenever needed. */
class LIBSAKUSEN_API Heightfield {
  private:
    Heightfield(
        uint32 horizontalResolution,
        uint32 verticalResolution,
        uint32 width,
        uint32 height,
        sint16* heightfield
      );
  public:
    Heightfield(
        uint32 horizontalResolution,
        uint32 verticalResolution,
        uint32 width,
        uint32 height
      );
    Heightfield(const Heightfield&);
    Heightfield& operator=(const Heightfield&);
    ~Heightfield();
  private:
    /** Dex between adjacent height samples in the heightfield */
    uint32 horizontalResolution;
    /** Dex represented by each vertical unit of the heightfield */
    uint32 verticalResolution;
    /** \brief The number of samples across the width of the heightfield.
     *
     * This should be 1 + map.width()/horizontalResolution */
    uint32 width;
    /** \brief The number of samples up the height of the heightfield.
     *
     * This should be 1 + map.height()/horizontalResolution */
    uint32 height;
    /** \brief Array of heights.
     *
     * The height at position (x*horizontalResolution, y*horizontalResolution)
     * is stored at heightfield[y*width+h] (where x and y are distnaces from
     * the map edges). */
    sint16* heightfield;

    void sanityCheck() const;
    inline uint32 dexToSampleFloorX(sint32 x) const;
    inline uint32 dexToSampleFloorY(sint32 y) const;
    inline uint32 dexToSampleCeilX(sint32 x) const;
    inline uint32 dexToSampleCeilY(sint32 y) const;
    inline sint32 sampleToDexX(uint32 x) const;
    inline sint32 sampleToDexY(uint32 y) const;
    
    inline sint32 getHeightAtSample(uint32 x, uint32 y) const {
      assert(x < width);
      assert(y < height);
      return heightfield[y*width + x];
    }
  public:
    /** \name accessors */
    //@{
    uint32 getHorizontalResolution() const { return horizontalResolution; }
    uint32 getVerticalResolution() const { return verticalResolution; }
    uint32 getWidth() const { return width; }
    uint32 getHeight() const { return height; }
    //@}

    sint32 getHeightAt(sint32 x, sint32 y) const;
    inline sint32 getHeightAt(const Point<sint32>& p) const {
      return getHeightAt(p.x, p.y);
    }
    sint32 getMaxHeightIn(const Rectangle<sint32>&) const;
    
    void store(OArchive&) const;
    static Heightfield load(IArchive&);
};

}

#endif // HEIGHTFIELD_H

