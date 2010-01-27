#ifndef LIBSAKUSEN__MORPHINGSAWWAVEFIELD_H
#define LIBSAKUSEN__MORPHINGSAWWAVEFIELD_H

#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>

#include "sawwavefield.h"

namespace sakusen {

/** \brief Time-varying noise field interpolated between SawWaveFields.
 *
 * This class generates successive Rank-dimensional SawWaveFields, and linearly
 * interpolates between them in time.  It is different from a
 * (Rank+1)-dimensional SawWaveField in that:
 *  - The temporal scale can be different from the spatial scale.
 *  - The LCG is reseeded for each new SawWaveField, rather than continuing in
 *    a very predictable manner.
 *  - Successive accesses must have non-decreasing time coordinates.
 *  - The temoral dimensions do not need to be specified in advance.
 */
template<std::size_t Rank, typename UniformRandomNumberGenerator>
class MorphingSawWaveField {
  BOOST_STATIC_ASSERT(Rank > 0);
  typedef SawWaveField<Rank> Subfield;
  public:
    typedef typename Subfield::dimension_type dimension_type;

    template<typename Range>
    MorphingSawWaveField(
        UniformRandomNumberGenerator& rng,
        uint32 const lSS,
        uint32 const lTS,
        Range const& dims
      ) :
      reseeder(
          &rng,
          boost::uniform_int<uint64>(0, std::numeric_limits<uint64>::max())
        ),
      logSpatialScale(lSS),
      logTemporalScale(lTS),
      timeBase(0),
      subfield0(reseeder(), logSpatialScale, dims),
      subfield1(reseeder(), logSpatialScale, dims)
    {
      assert(logTemporalScale < uint32(std::numeric_limits<Time>::digits));
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

    template<typename Range>
    sint32 valueAt(Time const t, Range const& coords) {
      assert(boost::size(coords) == Rank);
      Time const temporalScale = Time(1) << logTemporalScale;
      if (t < timeBase) {
        SAKUSEN_FATAL("time going backwards");
      } else if (t >= timeBase + temporalScale) {
        /* We need to reset one of both subfields */
        if (t < timeBase + (temporalScale<<1)) {
          /* We need to reset only one */
          subfield0.reseed(subfield1.seed());
          subfield1.reseed(reseeder());
        } else {
          /* We need to reset both */
          subfield0.reseed(reseeder());
          subfield1.reseed(reseeder());
        }
        timeBase = t & ~(temporalScale-1);
      }

      sint32 const sample0 = subfield0.valueAt(coords);
      sint32 const sample1 = subfield1.valueAt(coords);
      sint64 const remainder = t & (temporalScale-1);
      return (remainder * sample1 + (temporalScale-remainder) * sample0) >>
        logTemporalScale;
    }
  private:
    typedef boost::variate_generator<
      UniformRandomNumberGenerator*, boost::uniform_int<uint64>
    > Reseeder;
    Reseeder reseeder;
    uint32 logSpatialScale;
    uint32 logTemporalScale;
    Time timeBase;
    Subfield subfield0;
    Subfield subfield1;
};

}

#endif // LIBSAKUSEN__MORPHINGSAWWAVEFIELD_H

