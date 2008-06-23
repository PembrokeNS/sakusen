#ifndef GNU_EXTENSIONS_H
#define GNU_EXTENSIONS_H

#include "libsakusen-global.h"
#include <boost/functional/hash/hash.hpp>

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
/* Use the new TR1 hashmaps if we are using a new GCC which will complain
 * about hash_map */
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <ext/functional>

#define SAKUSEN_USE_UNORDERED_MAP

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

/* Now that the names don't even match we have to access the maps through a
 * metafunction... */

namespace sakusen {

#ifdef SAKUSEN_USE_UNORDERED_MAP

template<typename TValue, typename THash = boost::hash<TValue> >
struct u_set {
  typedef std::tr1::unordered_set<TValue, THash> type;
};

template<typename TKey, typename TValue, typename THash = boost::hash<TKey> >
struct u_map {
  typedef std::tr1::unordered_map<TKey, TValue, THash> type;
};

#else // SAKUSEN_USE_UNORDERED_MAP

template<typename TValue, typename THash = boost::hash<TValue> >
struct u_set {
  typedef __gnu_cxx::hash_set<TValue, THash> type;
};

template<typename TKey, typename TValue, typename THash = boost::hash<TKey> >
struct u_map {
  typedef __gnu_cxx::hash_map<TKey, TValue, THash> type;
};

#endif // SAKUSEN_USE_UNORDERED_MAP

}

#endif // GNU_EXTENSIONS_H

