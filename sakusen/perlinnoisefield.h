#ifndef LIBSAKUSEN__PERLINNOISEFIELD_H
#define LIBSAKUSEN__PERLINNOISEFIELD_H

#include "libsakusen-global.h"

#include <cstddef>
#include <vector>

#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/foreach.hpp>

#include "sawwavefield.h"

namespace sakusen {

/** \brief Perlin noise field built from SawWaveFields.
 *
 * PerlinNoiseField gives a field which is the weighted sum of several
 * (nominally independent) SawWaveFields.  The weight of each SawWaveField is
 * proportional to its scale (the distance between adjacent lattice points).
 * The largest-scale SawWaveField always has weight 1/2, and the scales of the
 * various SawWaveFields are always consecutive powers of 2.  Thus, as the
 * number of SawWaveFields increases, the range of possible values taken by the
 * PerlinNoiseField approaches the full sint32 range exponentially.  The mean
 * will always be a bit less than 0 (between -0.25 and -0.5), and the variance
 * will be highest near the lattice points of the SawWaveFields.
 */
template<std::size_t Rank>
class PerlinNoiseField {
  typedef SawWaveField<Rank> Subfield;
  public:
    typedef typename Subfield::dimension_type dimension_type;

    PerlinNoiseField(
        uint64 const seed,
        uint32 const logMinScale,
        uint32 const logMaxScale,
        dimension_type const dimX,
        dimension_type const dimY
      )
    {
      dimension_type dims[2] = { dimX, dimY };
      init(seed, logMinScale, logMaxScale, dims);
    }

    sint32 operator()(dimension_type x) const {
      BOOST_MPL_ASSERT_RELATION(Rank,==,1);
      dimension_type dims[1] = { x };
      return valueAt(dims);
    }

    sint32 operator()(dimension_type x, dimension_type y) const {
      dimension_type dims[2] = { x, y };
      return valueAt(dims);
    }

    sint32 operator()(
        dimension_type x, dimension_type y, dimension_type z
      ) const {
      dimension_type dims[3] = { x, y, z };
      return valueAt(dims);
    }
  private:
    template<typename Range>
    void init(
        uint64 seed,
        uint32 const logMinScale,
        uint32 const logMaxScale,
        Range const& dims
      ) {
      assert(boost::size(dims) == Rank);
      assert(logMinScale <= logMaxScale);
      assert(logMaxScale < uint32(std::numeric_limits<dimension_type>::digits));
      /* Each subfield will use an LCG RNG.  To provide seeds that are not
       * related in an LCG-ish way, we use an xorshift RNG to generate all
       * their seeds.  Use the first 64-bit xorshift RNG from the paper by
       * George Marsaglia (http://www.jstatsoft.org/v08/i14/paper).
       * seed must be non-zero for the xorshift RNG; replace zero with an
       * arbitrary 64-bit value (from /dev/urandom). */
      if (0 == seed) seed = 0x440d216c818e43f4;
      for (uint32 logScale = logMinScale;
          logScale != logMaxScale+1; ++logScale) {
        subfields.push_back(Subfield(seed, logScale, dims));
        seed ^= seed << 1; seed ^= seed >> 1; seed ^= seed << 54;
      }
    }
    template<typename Range>
    sint32 valueAt(Range const& coords) const {
      assert(boost::size(coords) == Rank);
      sint64 val = 0;
      BOOST_FOREACH(Subfield const& subfield, subfields) {
        sint64 const subfield_obs = subfield.valueAt(coords);
        val += subfield_obs;
        val /= 2;
      }
      /* Subfields always return values in the full sint32 range, so val is
       * also now in that range; we need not scale */
      return val;
    }

    std::vector<Subfield> subfields;
};

}

#endif // LIBSAKUSEN__PERLINNOISEFIELD_H

