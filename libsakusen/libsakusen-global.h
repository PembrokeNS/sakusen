#ifndef LIBSAKUSEN_GLOBAL_H
#define LIBSAKUSEN_GLOBAL_H

/* This header is a bit of a mess.  Try to contain as much of the
 * compiler- or platform-specific nonsense to this one header, and
 * thus prevent the mess spreading too far. */

/* First we have all the stuff to do with library/executable interfacing.
 * This is easy for gcc, but far from easy for MSVC */

#if defined(_MSC_VER)
  /* The __declspec stuff for ensuring symbols are exported from DLLs and
   * imported back into libraries */
  #ifdef LIBSAKUSEN_EXPORTS
    #define LIBSAKUSEN_API   __declspec(dllexport)
    #define LIBSAKUSEN_EXIMP extern
  #else
    #define LIBSAKUSEN_API   __declspec(dllimport)
    #define LIBSAKUSEN_EXIMP
  #endif

  /* Macros for explicit export of STL containers.  Needed because of the
   * strange way Windows DLLs work */

  /* EXPORT does a full export of its argument and all member functions */
  #define EXPORT(ExportedType) \
    template class LIBSAKUSEN_API ExportedType;

  /* E_EXPORT does notexport the member functions.  Try this one first,
   * and fall back on EXPORT if you get linking errors */
  #define E_EXPORT(EExportedType) \
    LIBSAKUSEN_EXIMP EXPORT(EExportedType);
	/* This is another one that may need re-enabling at some point. 
	Non-dll interdface class used for basis of dll-interface class. 
	Basically, all the STLport containers are based on other containers that 
	specialise in various ways for speed, and MSVC complains that the base classes 
	aren't exported, but then they wouldn't be, would they? */
  #pragma warning (disable: 4275)

  #define EXPORT_VECTOR(ElementType) \
    LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API std::allocator< ElementType >; \
    LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API std::_STLP_alloc_proxy< class ElementType*, class ElementType, std::allocator< ElementType > >; \
    LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API std::_Vector_base< ElementType, std::allocator< ElementType > >; \
    E_EXPORT(std::vector< ElementType >);

  #define EXPORT_LIST(ElementType) \
    EXPORT(std::list< ElementType >);

  /* Note that the first line in the following definition is "morally" just
   * E_EXPORT(std::_Deque_val< ElementType, std::allocator< ElementType > >)
   * but of course that doesn't work because macro arguments can't contain
   * commas, so it must be written out explicitly.  Similar things happen below
   * and above */
  #define EXPORT_QUEUE(ElementType) \
	LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API std::deque< ElementType, std::allocator< ElementType > >;

  /*EXPORT(std::deque< ElementType >) \ 
	EXPORT(std::queue< ElementType >) */

  /* This macro for exporting maps doesn't work properly. */
  #define EXPORT_MAP(KeyType, ValueType) \
    LIBSAKUSEN_EXIMP template LIBSAKUSEN_API std::map< KeyType, ValueType >::_Imp; \
    LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API std::map< KeyType, ValueType >;

  #define EXPORT_HASH_MAP(KeyType, ValueType) \
    LIBSAKUSEN_EXIMP template class LIBSAKUSEN_API __gnu_cxx::hash_map< KeyType, ValueType >;

  /* This warning is about debugging symbol names being truncated to 255
   * characters.  It's not at all important. */
  #pragma warning(disable: 4786)

  /* This warning is about members of exported classes not being exported.
   * In at least one case it was very important.  In the long run we should
   * do without disabling this warning. */
  #pragma warning(disable: 4251)
	
  /* Assert we need explicit template instantiation */
#if _MSC_VER<1400
  #define NEED_TEMPLATE_INSTANTIATION
#endif

  /* Define placeholder versions of operators to be inseerted into classes
   * which will be the element type of an explicitly instantiated templated
   * container */
  #define PLACEHOLDER_OPERATORS(Type) \
    bool operator<(const Type&) const { return true; } \
    bool operator>(const Type&) const { return true; } \
    bool operator==(const Type&) const { return true; } \
    bool operator!=(const Type&) const { return true; }
	#if (_MSC_VER<1310) //VC2003 or 2005.
	 /* Define a filler for the useful gcc __PRETTY_FUNCTION__ */
	  #define __PRETTY_FUNCTION__ "__PRETTY_FUNCTION__ not in MSVC6"
	#else
	//Defines a working alias for __PRETTY_FUNCTION__
		#define __PRETTY_FUNCTION__ __FUNCDNAME__
	#endif //VC2003/5

  /* Define file seperator */
  #define FILE_SEP "\\"
  #define FILE_SEPC '\\'

#elif defined(__GNUC__)

  /* In gcc, we have the following dll export thing, which will do nothing unless
   * we should happen to add the compiler options to alter the default
   * visibility. */

  #define LIBSAKUSEN_API __attribute__ ((visibility ("default")))

  /* We don't need placeholder operators */
  #define PLACEHOLDER_OPERATORS(Type)

  /* Define file seperator */
  #define FILE_SEP "/"
  #define FILE_SEPC '/'

#else

  /* If we're not compiling under gcc or MSVC then we don't know what to do */
  #error Compiler not supported

#endif

/* CONFIG_SUBDIR is the subdirectory of the users home directory where general
 * sakusen stuff happens. */
#define CONFIG_SUBDIR FILE_SEP ".sakusen"

/* Now define the streams used for debugging and error output */

#include <ostream>

extern LIBSAKUSEN_API std::ostream& debugStream;
extern LIBSAKUSEN_API std::ostream& errorStream;

#if defined (_MSC_VER)
  /* MS has no assert */
  #define assert(exp) \
    if (!(exp)) { \
      errorStream << __FILE__ << ":" << __LINE__ << ": assertion '" #exp \
        "' failed" << std::endl; \
      *(int*)NULL=0; /* HACK: silly way to simulate an 'abort()' */ \
    }
#endif // _MSC_VER

/* Define macros for debug and error output */

#define Debug(msg) \
  debugStream << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << \
    ": " << msg << std::endl;

#define Fatal(msg) \
  errorStream << __FILE__ << ":" << __LINE__ << ":" << __PRETTY_FUNCTION__ << \
    ": fatal error: " << msg << std::endl; \
  abort();

/* Obtain the types we need */

#if defined(_MSC_VER)
#if (_MSC_VER<1400)
  /* In MSVC there's no portable way to do it */
  typedef signed char          sint8;
  typedef unsigned char        uint8;
  typedef signed short int     sint16;
  typedef unsigned short int   uint16;
  typedef signed long int      sint32;
  typedef unsigned long int    uint32;
  typedef signed __int64       sint64;
  typedef unsigned __int64     uint64;
//	#include <uint64.h>
#else
 typedef signed __int8          sint8;
  typedef unsigned __int8        uint8;
  typedef signed __int16     sint16;
  typedef unsigned __int16   uint16;
  typedef signed __int32      sint32;
  typedef unsigned __int32    uint32;
  typedef signed __int64       sint64;
  typedef unsigned __int64     uint64;
#endif //_MSC_VER<1400
#else // _MSC_VER
  /* On a sane compiler there is a good way to do it */ 
  #include <stdint.h>
  typedef uint8_t  uint8;
  typedef int8_t   sint8;
  typedef uint16_t uint16;
  typedef int16_t  sint16;
  typedef uint32_t uint32;
  typedef int32_t  sint32;
  typedef uint64_t uint64;
  typedef int64_t  sint64;
#endif // _MSC_VER

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
  /* it also appears to lack a value of pi */
  #define M_PI 3.14159265358979
#endif

/* end platform-dependent code */

/* Finally, we declare some types for general usage */

#include <string>
typedef std::string String;
typedef uint32 Time;
typedef uint32 TimeSpan; /* We might want to allow negative TimeSpans too. */

#endif // LIBSAKUSEN_GLOBAL_H
