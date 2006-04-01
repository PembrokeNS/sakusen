#if !defined(__LIBDEFS_H)
#define LIBDEFS_H

/*
   {{{ includes 
 */
#include <mutils/mincludes.h>
#include <mutils/mglobal.h>
#include <mutils/mutils.h>
#include <mutils/mtypes.h>

#ifdef LIBMHASH_EXPORTS
#define WIN32DLL_DEFINE __declspec( dllexport)
#else
# define WIN32DLL_DEFINE
#endif

#define RAND32 (mutils_word32) ((mutils_word32)rand() << 17 ^ (mutils_word32)rand() << 9 ^ rand())

#endif
