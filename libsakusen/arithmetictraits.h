#ifndef LIBSAKUSEN__ARITHMETICTRAITS_H
#define LIBSAKUSEN__ARITHMETICTRAITS_H

#include "libsakusen-global.h"
#include <limits>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_signed.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/int.hpp>

namespace sakusen {

template<typename T, typename Enable = void>
struct ArithmeticTraits;

template<typename T>
struct ArithmeticTraits<
    T, typename boost::enable_if<boost::is_unsigned<T> >::type
  > {
  typedef typename boost::make_unsigned<T>::type unsign;
  typedef typename boost::make_signed<T>::type sign;
  typedef uint64 widest;
  typedef uint8 narrowest;
};

template<typename T>
struct ArithmeticTraits<
    T, typename boost::enable_if<boost::is_signed<T> >::type
  > {
  typedef typename boost::make_unsigned<T>::type unsign;
  typedef typename boost::make_signed<T>::type sign;
  typedef uint64 widest;
  typedef uint8 narrowest;
};

template<>
struct ArithmeticTraits<double, void>
{
  typedef double unsign;
  typedef double sign;
  typedef double widest;
};

#ifdef SWIG
%template(UArithmeticTraits8) ArithmeticTraits<uint8, void>;
%template(SArithmeticTraits8) ArithmeticTraits<sint8, void>;
%template(UArithmeticTraits16) ArithmeticTraits<uint16, void>;
%template(SArithmeticTraits16) ArithmeticTraits<sint16, void>;
%template(UArithmeticTraits32) ArithmeticTraits<uint32, void>;
%template(SArithmeticTraits32) ArithmeticTraits<sint32, void>;
%template(UArithmeticTraits64) ArithmeticTraits<uint64, void>;
%template(SArithmeticTraits64) ArithmeticTraits<sint64, void>;

%template(ArithmeticTraitsDouble) ArithmeticTraits<double>;
#endif /*SWIG*/

/** \brief Determines whether one integer type can be safely cast to another
 *
 * If both T and U are integer types, then this Metafunction defines typedef
 * member type to be true iff U can be cast to T without any loss or change to
 * the data.
 *
 * If T is double and U is an integer type then type is true unless U is a
 * 64-bit type  (It could be arranged for this to be "iff all U values are
 * representible in a double", but that might not be the same on all platforms).
 *
 * If U is double and T is an integer type then type is false.
 *
 * If both U and T are double then type is true.
 *
 * If one of U and T is neither an integer type nor double then the behaviour
 * is undefined.
 * 
 * For example, LosslessCast<sint32, uint8>::type is boost::mpl::true_ but
 * LosslessCast<sint32, uint32>::type is boost::mpl::false_
 */
template<typename T, typename U>
struct LosslessCast {
  typedef typename boost::mpl::and_<
      typename boost::mpl::less_equal<
        typename boost::mpl::int_<std::numeric_limits<U>::digits>,
        typename boost::mpl::int_<std::numeric_limits<T>::digits>
      >::type,
      typename boost::mpl::or_<
        typename boost::mpl::bool_<
          std::numeric_limits<T>::is_signed
        >,
        typename boost::mpl::not_<
          typename boost::mpl::bool_<
            std::numeric_limits<U>::is_signed
          >
        >::type
      >::type
    >::type type;
};

template<typename U>
struct LosslessCast<double, U> {
  typedef typename boost::mpl::less_equal<
      typename boost::mpl::int_<std::numeric_limits<U>::digits>,
      boost::mpl::int_<32>
    >::type type;
};

template<typename T>
struct LosslessCast<T, double> {
  typedef boost::mpl::false_ type;
};

template<>
struct LosslessCast<double, double> {
  typedef boost::mpl::true_ type;
};

}

#endif

