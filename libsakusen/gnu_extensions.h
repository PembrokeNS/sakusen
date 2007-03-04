#ifndef GNU_EXTENSIONS_H
#define GNU_EXTENSIONS_H

#include "libsakusen-global.h"

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/functional>

#else// STLport has its own versions of hash_map, etc.

#define __gnu_cxx std

#include <hash_map>
#include <hash_set>
#include <functional>

#endif // __GNUC__

#endif // GNU_EXTENSIONS_H

