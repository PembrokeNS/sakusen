#ifndef GNU_EXTENSIONS_H
#define GNU_EXTENSIONS_H

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#include <ext/hash_map>
#include <ext/hash_set>

#else// STLport has its own versions of hash_map, etc.

#include <hash_map>
#include <hash_set>

#endif // __GNUC__

#endif // GNU_EXTENSIONS_H

