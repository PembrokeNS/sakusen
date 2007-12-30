#ifndef HEIGHTFIELD_H
#define HEIGHTFIELD_H

#include "libsakusen-global.h"

#include <boost/multi_array.hpp>

#include "point.h"
#include "rectangle.h"
#include "iarchive.h"
#include "oarchive.h"
#include "iheightfield.h"
#include "image.h"

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
class LIBSAKUSEN_API Heightfield : public IHeightfield {
  protected:
    typedef boost::multi_array<sint16, 2> hf_type;
    Heightfield(
        uint32 xyResolution,
        uint32 zResolution,
        uint32 width,
        uint32 height,
        const hf_type& heightfield
      );
  public:
    Heightfield(
        uint32 xyResolution,
        uint32 zResolution,
        uint32 width,
        uint32 height
      );
    Heightfield(
        uint32 xyResolution,
        uint32 zResolution,
        const String& imagePath,
        const Image::ConstPtr& image
      );
    Heightfield& operator=(const Heightfield&);
  private:
    /** \brief If loaded from an image, the (probably relative) sakusen path
     * to it */
    String imagePath;
    /** Dex between adjacent height samples in the heightfield */
    uint32 xyResolution;
    /** Dex represented by each vertical unit of the heightfield */
    uint32 zResolution;
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
     * is stored at heightfield[x][y]. */
    hf_type heightfield;

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
      return heightfield[x][y] * sint32(zResolution);
    }

    double intersectRayInCell(
        const Ray&,
        double minimum,
        double extent,
        uint32 x,
        uint32 y
      ) const;
  public:
    /** \name accessors */
    //@{
    uint32 getXYResolution() const { return xyResolution; }
    uint32 getZResolution() const { return zResolution; }
    uint32 getWidth() const { return width; }
    uint32 getHeight() const { return height; }
    //@}

    sint32 getHeightAt(sint32 x, sint32 y) const;
    /** \brief Get the height of the heightfield at a given position.
     *
     * Given a position in space, returns the
     * height on the vertical line through that position, in dex.
     *
     * \note \c p.z is irrelevant.
     */
    inline sint32 getHeightAt(const Position& p) const {
      return getHeightAt(p.x, p.y);
    }

    inline sint32 getApproxHeightAt(sint32 x, sint32 y) const;
    inline sint32 getApproxHeightAt(const Position& p) const;

    sint32 getMaxHeightIn(const Rectangle<sint32>&) const;
    double intersectRay(const Ray&, double extent) const;
    
    void store(OArchive&) const;
    static Heightfield load(IArchive&, const DeserializationContext&);
};

}

#endif // HEIGHTFIELD_H

