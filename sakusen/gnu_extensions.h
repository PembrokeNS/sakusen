#ifndef LIBSAKUSEN__GNU_EXTENSIONS_H
#define LIBSAKUSEN__GNU_EXTENSIONS_H

#include <sakusen/libsakusen-global.h>
#include <boost/functional/hash/hash.hpp>

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)
/* Use the new TR1 hashmaps if we are using a new GCC which will complain
 * about hash_map */
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <ext/functional>
#include <ext/numeric>

#define SAKUSEN_USE_UNORDERED_MAP

#else

#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/functional>
#include <ext/numeric>

#endif // __GNUC__ > 4

#else

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/functional.hpp>

#define SAKUSEN_USE_UNORDERED_MAP

#endif // __GNUC__

/* Now that the names don't even match we have to access the maps through a
 * metafunction... */

namespace sakusen {

/** \bug Will need a variant of this for MSVC */
template<typename T, typename Integer>
inline T power(T c, Integer n) { return __gnu_cxx::power(c, n); }

#ifdef SAKUSEN_USE_UNORDERED_MAP

#ifdef _MSC_VER
  #define SAKUSEN_UNORDERED_MAP_NAMESPACE boost
#else
  #define SAKUSEN_UNORDERED_MAP_NAMESPACE std::tr1
#endif

template<typename TValue, typename THash = boost::hash<TValue> >
struct u_set {
  typedef SAKUSEN_UNORDERED_MAP_NAMESPACE::unordered_set<TValue, THash> type;
};

template<typename TKey, typename TValue, typename THash = boost::hash<TKey> >
struct u_map {
  typedef SAKUSEN_UNORDERED_MAP_NAMESPACE::unordered_map<TKey, TValue, THash>
    type;
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

#endif // LIBSAKUSEN__GNU_EXTENSIONS_H

