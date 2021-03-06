/* include/mutils/mhash_config.h.in.  Generated from configure.in by autoheader.  */

/* "Adler32 Algorithm" */
#undef ENABLE_ADLER32

/* "CRC32 Algorithm" */
#undef ENABLE_CRC32

/* "GOST Algorithm" */
#undef ENABLE_GOST

/* "Haval Algorithm" */
#undef ENABLE_HAVAL

/* "MD-2 Algorithm" */
#undef ENABLE_MD2

/* "MD-4 Algorithm" */
#undef ENABLE_MD4

/* "MD-5 Algorithm" */
#define ENABLE_MD5 1

/* "RIPE-MD - RIPE Substitute for MD" */
#undef ENABLE_RIPEMD

/* "SHA-1 Algorithm" */
#define ENABLE_SHA1 1

/* "SHA-224 and SHA-256 - SHA-2 Algorithms" */
#define ENABLE_SHA256_SHA224 1

/* "SHA-384 and SHA-512 - SHA-2 Algorithms" */
#undef ENABLE_SHA512_SHA384

/* "Snefru Algorithm" */
#undef ENABLE_SNEFRU

/* "Tiger Algorithm" */
#undef ENABLE_TIGER

/* "Whirlpool Algorithm" */
#undef ENABLE_WHIRLPOOL

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the `bcopy' function. */
#undef HAVE_BCOPY

/* Define to 1 if you have the `bzero' function. */
#undef HAVE_BZERO

/* Define to 1 if you have the <ctype.h> header file. */
#define HAVE_CTYPE_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the `fcntl' function. */
#undef HAVE_FCNTL

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fsync' function. */
#undef HAVE_FSYNC

/* Define to 1 if you have the <inttypes.h> header file. */
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the `lstat' function. */
#undef HAVE_LSTAT

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the `sigaction' function. */
#undef HAVE_SIGACTION

/* Define to 1 if you have the `signal' function. */
#define HAVE_SIGNAL 1

/* Define to 1 if you have the <signal.h> header file. */
#define HAVE_SIGNAL_H 1

/* Define to 1 if you have the `stat' function. */
#define HAVE_STAT 1

/* Define to 1 if stdbool.h conforms to C99. */
#undef HAVE_STDBOOL_H

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `umask' function. */
#define HAVE_UMASK 1

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the `utime' function. */
#undef HAVE_UTIME

/* Define to 1 if you have the <utime.h> header file. */
#undef HAVE_UTIME_H

/* Define to 1 if you have the <values.h> header file. */
#undef HAVE_VALUES_H

/* Define to 1 if the system has the type `_Bool'. */
//#undef HAVE__BOOL

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
//#undef NO_MINUS_C_MINUS_O

/* Name of package */
//#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
//#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
//#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
//#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
//#undef PACKAGE_TARNAME

/* Define to the version of this package. */
//#undef PACKAGE_VERSION

/* Define to 1 if the C compiler supports function prototypes. */
#define PROTOTYPES 1

/* The size of a `', as computed by sizeof. */
//#undef SIZEOF_

/* The size of a `unsigned char', as computed by sizeof. */
//#undef SIZEOF_UNSIGNED_CHAR

/* The size of a `unsigned int', as computed by sizeof. */
//#undef SIZEOF_UNSIGNED_INT

/* The size of a `unsigned long int', as computed by sizeof. */
//#undef SIZEOF_UNSIGNED_LONG_INT

/* The size of a `unsigned long long int', as computed by sizeof. */
//#undef SIZEOF_UNSIGNED_LONG_LONG_INT

/* The size of a `unsigned short int', as computed by sizeof. */
//#undef SIZEOF_UNSIGNED_SHORT_INT

/* Define to 1 if you have the ANSI C header files. */
//#undef STDC_HEADERS

/* Version number of package */
//#undef VERSION

/* Define if using the dmalloc debugging malloc package */
//#undef WITH_DMALLOC

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#undef WORDS_BIGENDIAN

/* Define to 1 if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
# undef _ALL_SOURCE
#endif

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# undef _GNU_SOURCE
#endif

/* Define to 1 if on MINIX. */
#undef _MINIX

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
#undef _POSIX_1_SOURCE

/* Define to 1 if you need to in order for `stat' and other things to work. */
#undef _POSIX_SOURCE

/* Define like PROTOTYPES; this can be used by system headers. */
#define __PROTOTYPES 1

/* Define to empty if `const' does not conform to ANSI C. */
//#undef const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
//#ifndef __cplusplus
//#undef inline
//#endif

/* Define to rpl_malloc if the replacement function should be used. */
//#undef malloc

/* Define to `unsigned' if <sys/types.h> does not define. */
//#undef size_t

#ifdef _MSC_VER
    #define uint64_t unsigned __int64
    #define uint32_t unsigned __int32
    #define uint16_t unsigned __int16
    #define uint8_t unsigned __int8
#endif

//If you have error.h
#define HAVE_ERROR_H
