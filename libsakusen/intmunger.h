#ifndef INTMUNGER_H
#define INTMUNGER_H

#include "libsakusen-global.h"
#include <limits>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/int.hpp>

namespace sakusen {

template<typename T>
struct IntMunger { };

template<>
struct IntMunger<uint8> {
  typedef uint8 unsign;
  typedef sint8 sign;
  typedef uint16 wider;
  typedef uint64 widest;
  typedef uint8 narrowest;
};

template<>
struct IntMunger<sint8> {
  typedef uint8 unsign;
  typedef sint8 sign;
  typedef sint16 wider;
  typedef sint64 widest;
  typedef sint8 narrowest;
};

template<>
struct IntMunger<uint16> {
  typedef uint16 unsign;
  typedef sint16 sign;
  typedef uint32 wider;
  typedef uint64 widest;
  typedef uint8 narrower;
  typedef uint8 narrowest;
};

template<>
struct IntMunger<sint16> {
  typedef uint16 unsign;
  typedef sint16 sign;
  typedef sint32 wider;
  typedef sint64 widest;
  typedef sint8 narrower;
  typedef sint8 narrowest;
};

template<>
struct IntMunger<uint32> {
  typedef uint32 unsign;
  typedef sint32 sign;
  typedef uint64 wider;
  typedef uint64 widest;
  typedef uint16 narrower;
  typedef uint8 narrowest;
};

template<>
struct IntMunger<sint32> {
  typedef uint32 unsign;
  typedef sint32 sign;
  typedef sint64 wider;
  typedef sint64 widest;
  typedef sint16 narrower;
  typedef sint8 narrowest;
};

template<>
struct IntMunger<uint64> {
  typedef uint64 unsign;
  typedef sint64 sign;
  typedef uint64 widest;
  typedef uint32 narrower;
  typedef uint8 narrowest;
};

template<>
struct IntMunger<sint64> {
  typedef uint64 unsign;
  typedef sint64 sign;
  typedef sint64 widest;
  typedef sint32 narrower;
  typedef sint8 narrowest;
};

template<>
struct IntMunger<double>
{
  typedef double unsign;
  typedef double sign;
  typedef double widest;
};

#ifdef SWIG
%template(UIntMunger8) IntMunger<uint8>;
%template(SIntMunger8) IntMunger<sint8>;
%template(UIntMunger16) IntMunger<uint16>;
%template(SIntMunger16) IntMunger<sint16>;
%template(UIntMunger32) IntMunger<uint32>;
%template(SIntMunger32) IntMunger<sint32>;
%template(UIntMunger64) IntMunger<uint64>;
%template(SIntMunger64) IntMunger<sint64>;

%template(IntMungerDouble) IntMunger<double>;
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

