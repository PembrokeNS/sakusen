#ifndef IDBASE_H
#define IDBASE_H

#include <boost/static_assert.hpp>

#include "gnu_extensions.h"
#include "stringutils.h"

namespace sakusen {

class IArchive;
class OArchive;

/** \brief Helper class to allow id-style classes to be easily constructed.
 *
 * Classes can inherit from this class and have semantics appropriate to an id.
 * The template parameters should be some integral type and the type of the
 * derived class itself.  See PlayerId for an example.
 *
 * IdBase adds as much appropriate functionality as it can to the derived
 * class through the use of friend function definitions and static methods.
 * 
 * One thing that cannot be added here is a specialization of hash for the
 * derived class.  Such must be added manually if desired.
 *
 * It is guaranteed that if an instance is default-constructed and then
 * incremented, all values obtained will be distinct until an invalid value is
 * reached (as determined by the \c valid method); after that duplicates may be
 * observed.
 */
template<typename TInteger, typename TDerived>
class IdBase {
  BOOST_STATIC_ASSERT((std::numeric_limits<TInteger>::is_integer));
  friend class IArchive;
  friend class OArchive;
  public:
    /** \brief Type which this is a wrapper for. */
    typedef TInteger internal_type;
  protected:
    /** \brief Default constructor always produces the same, valid id */
    IdBase() : val(0) {}
  private:
    TInteger val;
  public:
    /** \brief Implicit cast to internal type.
     *
     * This allows this to be treated as an integer for such things as
     * comparisons and array indexing. */
    operator TInteger() const { return val; }
    /** \brief Determine if this id is valid. */
    bool valid() const { return val != TInteger(-1); }
    /** \brief Convert to string. 
     *
     * \see IdBase::fromString */
    String toString() const { return numToString(val); }
    /** \brief Store in OArchive.
     *
     * This should not be needed to be used directly; simply using operator<<
     * is probably easier. */
    void store(OArchive& archive) const { archive << *this; }
    /** \brief Load from IArchive.
     *
     * This should not be needed to be used directly; simply using operator>>
     * is probably easier. */
    static TDerived load(IArchive& archive) {
      TDerived b;
      archive >> b;
      return b;
    }

    /** \brief Construct an invalid id. */
    static TDerived invalid() {
      TDerived temp; temp.val=TInteger(-1); return temp;
    }
    /** \brief Construct from string.
     *
     * It is guaranteed that converting an id to a string and back again will
     * yield the original value.
     *
     * \see IdBase::toString. */
    static TDerived fromString(const String& s) {
      return fromInteger(numFromString<TInteger>(s));
    }
    /** \brief Construct from internal integer representation.
     *
     * The need to use this function is a bad sign; it shows excessive
     * dependence on the internal representation. */
    static TDerived fromInteger(const TInteger i) {
      TDerived b;
      b.val = i;
      return b;
    }

    friend TDerived& operator++(TDerived& b) { ++b.val; return b; }
    friend TDerived operator++(TDerived& b, int) {
      TDerived temp(b); ++b; return temp;
    }
    friend bool operator==(TDerived l, TDerived r) { return l.val == r.val; }
    friend bool operator!=(TDerived l, TDerived r) { return l.val != r.val; }
};

}

#endif // IDBASE_H

