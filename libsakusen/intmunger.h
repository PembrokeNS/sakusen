#ifndef INTMUNGER_H
#define INTMUNGER_H

#include "libsakusen-global.h"

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
  typedef uint32 unsign;
  typedef sint32 sign;
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
}

#endif

