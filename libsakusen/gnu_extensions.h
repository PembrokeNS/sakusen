#ifndef GNU_EXTENSIONS_H
#define GNU_EXTENSIONS_H

#include "libsakusen-global.h"

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
// Use the new TR1 hashmaps if we are using a new GCC which will complain about hash_map
// TODO: make the code actually use unordered_map etc.
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <ext/functional>

#else

#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/functional>

#endif // __GNUC__ > 4

#else// STLport has its own versions of hash_map, etc.

#define __gnu_cxx std

#include <hash_map>
#include <hash_set>
#include <functional>

#endif // __GNUC__

#endif // GNU_EXTENSIONS_H

