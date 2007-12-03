#ifndef LIBSAKUSEN_GLOBAL_H
#define LIBSAKUSEN_GLOBAL_H

/* This header is a bit of a mess.  Try to contain as much of the
 * compiler- or platform-specific nonsense to this one header, and
 * thus prevent the mess spreading too far. */

/* First we have all the stuff to do with library/executable interfacing. */

#if defined(_MSC_VER)

  #if(_MSC_VER<1310)
    /*1310 is VC2003. None of this will work with earlier compilers, and later 
    ones are free, so there is really no reason to not upgrade */
    #error Need at least MSVC 7.1 to work.
  #endif 

  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
//#if 0 
//If libsakusen was a dll, this would specify what is in the dll interface.
//However, in order to make life easier, I am making it a static library.
//Alas this does not work, because of C4251 and John doing funky things with global pointers. 
  #ifdef LIBSAKUSEN_EXPORTS
    #define LIBSAKUSEN_API   __declspec(dllexport)
    #define LIBSAKUSEN_EXIMP extern
    #define LIBSAKUSEN_METHOD_DEFINITIONS
  #else
    #define LIBSAKUSEN_API   __declspec(dllimport)
    #define LIBSAKUSEN_EXIMP
  #endif

  /*4251: This warning is about members of exported classes not being exported.
   * This is mostly to do with stuff from std, but in at least two cases it 
   * was very important.  This warning should be enabled every now and again for checking. */
  /*4275: This warning is about exported classes having non-exported classes as a base.
   * Mostly to do with boost libraries. */
  #pragma warning(disable: 4715 4345 4275 4251)
  /* 4345 is a warning about a change from VS 2003. We don't care. 
   * 4715: Disable "not all code paths return a value" warning because the
   * compiler is too stupid to exhibit it properly */

  //Alias of __PRETTY_FUNCTION for 
  #define __PRETTY_FUNCTION__ __FUNCDNAME__

  /* Try to prevent min and max macros being defined.  They cause name
   * clashes, and the inline function versions are better anyway */
  #define NOMINMAX
  
  /*Define types the MSVC way */
  typedef signed __int8          sint8;
  typedef unsigned __int8        uint8;
  typedef signed __int16     sint16;
  typedef unsigned __int16   uint16;
  typedef signed __int32      sint32;
  typedef unsigned __int32    uint32;
  typedef signed __int64       sint64;
  typedef unsigned __int64     uint64;

  #define _USE_MATH_DEFINES 1;
  //Carries several maths constants with it.

#elif defined(__GNUC__)

  /* In gcc, we have the following dll export thing, which will do nothing
   * unless we should happen to add the compiler options to alter the default
   * visibility. */

  #if (__GNUC__ >= 4)
    #define LIBSAKUSEN_API __attribute__ ((visibility ("default")))
  #else
    #define LIBSAKUSEN_API
  #endif

  #define LIBSAKUSEN_METHOD_DEFINITIONS

  /* Define types the gcc way. */
  #include <stdint.h>
  typedef uint8_t  uint8;
  typedef int8_t   sint8;
  typedef uint16_t uint16;
  typedef int16_t  sint16;
  typedef uint32_t uint32;
  typedef int32_t  sint32;
  typedef uint64_t uint64;
  typedef int64_t  sint64;
#elif defined(SWIG)
  %include "stdint.i"
  typedef uint8_t  uint8;
  typedef int8_t   sint8;
  typedef uint16_t uint16;
  typedef int16_t  sint16;
  typedef uint32_t uint32;
  typedef int32_t  sint32;
  typedef uint64_t uint64;
  typedef int64_t  sint64;
#else

  /* If we're not compiling under gcc or MSVC then we don't know what to do */
  #error Compiler not supported

#endif

/* CONFIG_SUBDIR is the subdirectory of the users home directory where general
 * sakusen stuff happens. */
#define CONFIG_SUBDIR ".sakusen"

#include <cassert>

/* Now define the streams used for debugging and error output */

#include <ostream>

extern LIBSAKUSEN_API std::ostream& debugStream;
extern LIBSAKUSEN_API std::ostream& errorStream;

/* Define macros for debug and error output.  They're inside loops to absorb
 * the semicolon that should be used after calling them. */

#ifndef NDEBUG
  #define Debug(msg) \
    do { \
      debugStream << __FILE__ << ":" << __LINE__ << ":" << \
        __PRETTY_FUNCTION__ <<  ": " << msg << std::endl; \
    } while(false)

  #define QDebug(msg) \
    do { \
      debugStream << __FILE__ << ":" << __LINE__ << ":" << msg << std::endl; \
    } while(false)
#else
  #define Debug(msg)
  #define QDebug(msg)
#endif

#define Fatal(msg) \
  do { \
    errorStream << __FILE__ << ":" << __LINE__ << ":" << \
      __PRETTY_FUNCTION__ <<  ": fatal error: " << msg << std::endl; \
    abort(); \
  } while(false)


#include <cmath>

#if defined(_MSC_VER)
  /* The MS math.h lacks a truncate function */
  inline double trunc(double x) {
    return ( x > 0 ? floor(x) : ceil(x) );
  }
  /* And a round function.  */
  inline double round(double x)
  {
    return ( x < 0 ? ceil(x-0.5) : floor (x+0.5) );
  }

  //isnan -> _isnan for MSVC
#define isnan _isnan

    #ifdef _DEBUG
        #pragma warning(disable: 4503)
    //Decorated function name limit exceeded. Does not affect correctness of the program.
    #endif

#endif

/* end platform-dependent code */

/* Finally, we declare some types for general usage */

#include <stdexcept>
#include <string>
typedef std::string String;

namespace sakusen {

/** \brief Typedef for Times, primarily to make it clear when Times are
 * being manipulated */
typedef uint32 Time;
/** \brief Typedef for TimeSpans (i.e. the interval between two Times)
 * primarily to make it clear when such are being manipulated
 *
 * \todo We might want to allow negative TimeSpans too. */
typedef uint32 TimeSpan;

/** \brief Sign function
 *
 * \param x value to determine sign of
 *
 * \return 1, -1 or 0 according to the sign of x. */
inline int sgn(double x) {
  if (x>0) {
    return 1;
  } else if (x<0) {
    return -1;
  } else {
    return 0;
  }
}

}

#endif // LIBSAKUSEN_GLOBAL_H

