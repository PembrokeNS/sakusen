#ifndef LIBSAKUSEN__LCGITERATOR_H
#define LIBSAKUSEN__LCGITERATOR_H

#include <sakusen/libsakusen-global.h>

#include <boost/iterator/iterator_facade.hpp>

#include <sakusen/gnu_extensions.h>

namespace sakusen {

namespace detail {

/* metafunction to compute powers; for now settle for uint64s only; could be
 * made a proper MPL metafunction relatively easily. */
template<uint64 x, uint64 n>
struct Power {
  static uint64 const t = Power<x, (n>>1)>::value;
  static uint64 const value = t*t*((n&1) ? x : 1);
};

template<uint64 x>
struct Power<x, 0> {
  static uint64 const value = 1;
};

}

/** \brief Mod-2^64 LCG with zero additive.
 *
 * This iterator allows exploration of some odd values mod 2^64.  For
 * maximum speed it uses zero additive (and thus can't have period 2^64).  It
 * ensures that the period is 2^62 (I think that's the best possible)
 * provided that the state is odd (there are cycles of various power-of-two
 * lengths when the state is even).
 *
 * This is a poor-quality RNG, but it does allow efficient random access
 * through the state space, allowing this to be a RandomAccessIterator.
 */
template<uint64 Multiplier>
class LcgIterator :
  public boost::iterator_facade<
    LcgIterator<Multiplier>,
    uint64 const,
    boost::random_access_traversal_tag,
    uint64 const&,
    sint64
  >
{
  BOOST_MPL_ASSERT_RELATION(
      (detail::Power<Multiplier, uint64(1)<<62>::value),==,1
    );
  /* Assertion failure here means that the Multiplier needs to be changed to
   * ensure full-cycle LCG.  The cast to int64 is to make clang happy (because
   * otherwise there's an implicit sign change) */
  BOOST_MPL_ASSERT_RELATION(
      sint64(detail::Power<Multiplier, uint64(1)<<61>::value),!=,1
    );
  public:
    LcgIterator(uint64 const s) : state(s) { assert(state & 1); }

    void reseed(uint64 const s) { assert(state & 1); state = s; }
  private:
    friend class boost::iterator_core_access;

    uint64 const& dereference() const { return state; }

    void advance(uint64 n) {
      /* Since the cycle is a power-of-2 length, advancing by negative numbers
       * works fine (although might be slower than necessary) */
      state *= power(Multiplier, n);
    }
    uint64 state;
};

}

#endif // LIBSAKUSEN__LCGITERATOR_H

