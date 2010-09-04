#ifndef LIBSAKUSEN__SAWWAVEFIELD_H
#define LIBSAKUSEN__SAWWAVEFIELD_H

#include <limits>

#include <boost/type_traits/is_integral.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/size.hpp>

#include "lcgiterator.h"
#include "mathsutils.h"

namespace sakusen {

namespace detail {
  template<std::size_t RankLeft>
  struct Interpolater {
    template<typename SampleIterator, typename CoordsIterator>
    sint32 operator()(
        SampleIterator it,
        uint32 const logScale,
        uint64 const* const strides,
        CoordsIterator const coordsBegin,
        CoordsIterator coordsEnd
      ) const {
      assert(std::distance(coordsBegin, coordsEnd) == RankLeft);
      --coordsEnd;
      typedef typename std::iterator_traits<CoordsIterator>::value_type
        dimension_type;
      dimension_type const last_coord = *coordsEnd;
      dimension_type const last_sample_coord = last_coord >> logScale;
      /* This might be fractionally more efficient if we stored offsets
       * rather than iterators, and only altered the iterator at the end.  Or
       * it might not. */
      it += last_sample_coord * strides[RankLeft-1];
      Interpolater<RankLeft-1> subinterpolater;
      sint32 const sample0 = subinterpolater(
          it, logScale, strides, coordsBegin, coordsEnd
        );
      it += strides[RankLeft - 1];
      sint32 const sample1 = subinterpolater(
          it, logScale, strides, coordsBegin, coordsEnd
        );
      /* Having computed the value at each end of the interval, do the
       * interpolation */
      sint64 const remainder = last_coord & ((1U<<logScale)-1);
      return (remainder * sample1 + ((1U<<logScale)-remainder) * sample0) >>
        logScale;
    }
  };

  template<>
  struct Interpolater<0> {
    template<typename SampleIterator, typename CoordsIterator>
    sint32 operator()(
        SampleIterator it,
        uint32 const /*logScale*/,
        uint64 const* const /*strides*/,
        CoordsIterator const /*coordsBegin*/,
        CoordsIterator /*coordsEnd*/
      ) const {
      /* Take top 32 bits of 64-bit state (low bits have very poor randomness).
       */
      return *it>>32;
    }
  };
}

/** \brief Random noise field built with multilinear interpolation.
 *
 * SawWaveField uses an LCG to generate random values to take at points of a
 * square axis-aligned Rank-dimensional lattice, and interpolates linearly
 * between them.  (So, in one dimension it looks a bit like a swa wave).  The
 * range of values taken by the field is the full range of sint32, with mean
 * -0.5, and variance varying (maximal at the lattice points, where the
 * distribution is uniform across all sint32 values; lower at points further
 * from the lattice points).
 *
 * The non-zero mean could be made zero by adding a bit to the LCG
 * observations before scaling them down if it was deemed important.
 */
template<std::size_t Rank>
class SawWaveField {
  BOOST_STATIC_ASSERT(Rank > 0);
  public:
    /* Don't use size_t since that's less portable */
    typedef uint32 dimension_type;

    template<typename Range>
    SawWaveField(
        uint64 const seed,
        uint32 const lS,
        Range const& dims
      ) :
      logScale(lS),
      /* Need LCG state to be odd */
      iterator(seed | 1)
    {
      assert(logScale < uint32(std::numeric_limits<dimension_type>::digits));
      assert(boost::size(dims) == Rank);
      uint64 stride = 1;
      /** \todo We should probably check that all dimensions fit in a uint32 */
      for (std::size_t i = 0; i<Rank; ++i) {
        strides[i] = stride;
        uint64 const sample_dim = mathsUtils_divideRoundUp<uint64>(
              dims[i], dimension_type(1)<<logScale
            ) + 1;
        /* Be careful of overflow */
        assert(uint64(-1)/sample_dim >= stride);
        stride *= sample_dim;
      }
    }

    sint32 operator()(dimension_type const x, dimension_type const y) const {
      BOOST_MPL_ASSERT_RELATION(Rank,==,2);
      dimension_type coords[2] = { x, y };
      return valueAt(coords);
    }

    template<typename Range>
    sint32 valueAt(Range const& coords) const {
      assert(boost::size(coords) == Rank);
      return detail::Interpolater<Rank>()(
          iterator, logScale, strides, boost::begin(coords), boost::end(coords)
        );
    }

    uint64 seed() const { return *iterator; }

    void reseed(uint64 const seed) { iterator.reseed(seed | 1); }
  private:
    uint32 logScale;
    LcgIterator<0x7bec9cc1ef90f1a3> iterator;
    uint64 strides[Rank];
};

}

#endif // LIBSAKUSEN__SAWWAVEFIELD_H

