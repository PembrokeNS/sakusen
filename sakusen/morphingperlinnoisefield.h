#ifndef LIBSAKUSEN__MORPHINGPERLINNOISEFIELD_H
#define LIBSAKUSEN__MORPHINGPERLINNOISEFIELD_H

#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/foreach.hpp>

#include <sakusen/morphingsawwavefield.h>

namespace sakusen {

/** \brief Time-varying noise field formed from sums of MorphingSawWaveFields.
 *
 * This class is to MorphingSawWaveField as PerlinNoiseField is to
 * SawWaveField.  It adds together many such fields at varying scales.  Time
 * and space are scaled in parallel.
 */
template<std::size_t Rank, typename UniformRandomNumberGenerator>
class MorphingPerlinNoiseField {
  BOOST_STATIC_ASSERT(Rank > 0);
  typedef MorphingSawWaveField<Rank, UniformRandomNumberGenerator> Subfield;
  public:
    typedef typename Subfield::dimension_type dimension_type;

    MorphingPerlinNoiseField(
        UniformRandomNumberGenerator& rng,
        uint32 const logMinSpatialScale,
        uint32 const logMaxSpatialScale,
        uint32 const logMinTemporalScale,
        dimension_type const dimX,
        dimension_type const dimY
      )
    {
      dimension_type dims[2] = { dimX, dimY };
      init(
          rng, logMinSpatialScale, logMaxSpatialScale,
          logMinTemporalScale, dims
        );
    }

    MorphingPerlinNoiseField(
        UniformRandomNumberGenerator& rng,
        uint32 const logMinSpatialScale,
        uint32 const logMaxSpatialScale,
        uint32 const logMinTemporalScale,
        dimension_type const dimX,
        dimension_type const dimY,
        dimension_type const dimZ
      )
    {
      dimension_type dims[3] = { dimX, dimY, dimZ };
      init(
          rng, logMinSpatialScale, logMaxSpatialScale,
          logMinTemporalScale, dims
        );
    }

    sint32 operator()(
        Time const t,
        dimension_type const x
      ) const {
      BOOST_MPL_ASSERT_RELATION(Rank,==,1);
      dimension_type coords[1] = { x };
      return valueAt(t, coords);
    }

    sint32 operator()(
        Time const t,
        dimension_type const x,
        dimension_type const y
      ) {
      BOOST_MPL_ASSERT_RELATION(Rank,==,2);
      dimension_type coords[2] = { x, y };
      return valueAt(t, coords);
    }

    sint32 operator()(
        Time const t,
        dimension_type const x,
        dimension_type const y,
        dimension_type const z
      ) {
      BOOST_MPL_ASSERT_RELATION(Rank,==,3);
      dimension_type coords[3] = { x, y, z };
      return valueAt(t, coords);
    }
  private:
    template<typename Range>
    void init(
        UniformRandomNumberGenerator& rng,
        uint32 const logMinSpatialScale,
        uint32 const logMaxSpatialScale,
        uint32 const logMinTemporalScale,
        Range const& dims
      ) {
      assert(boost::size(dims) == Rank);
      assert(logMinSpatialScale <= logMaxSpatialScale);
      assert(logMaxSpatialScale <
          uint32(std::numeric_limits<dimension_type>::digits));

      for (uint32 logSpatialScale = logMinSpatialScale,
          logTemporalScale = logMinTemporalScale;
          logSpatialScale != logMaxSpatialScale+1;
          ++logSpatialScale, ++logTemporalScale) {
        subfields.push_back(
            Subfield(rng, logSpatialScale, logTemporalScale, dims)
          );
      }
    }

    template<typename Range>
    sint32 valueAt(Time const t, Range const& coords) {
      assert(boost::size(coords) == Rank);
      sint64 val = 0;
      BOOST_FOREACH(Subfield& subfield, subfields) {
        sint64 const subfield_obs = subfield.valueAt(t, coords);
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

#endif // LIBSAKUSEN__MORPHINGPERLINNOISEFIELD_H

