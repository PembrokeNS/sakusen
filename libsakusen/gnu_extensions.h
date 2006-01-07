#ifndef GNU_EXTENSIONS_H
#define GNU_EXTENSIONS_H

#ifdef __GNUC__

/* If the GNU extensions are available, then use them */
#include <ext/hash_map>
#include <ext/hash_set>

#else// STLport has its own versions of hash_map, etc.

#include <hash_map>
#include <hash_set>

#define __gnu_cxx std

/*#else */

#if 0
/* If not, then approximate them */
#include <map>
#include <set>

namespace __gnu_cxx {
  /* Do our own version of hash for the needed types  */
  template <typename Type>
    class hash {
    };

  template <>
    class hash<const char*> {
      public:
        inline size_t operator()(char const* const& s) const {
          char const* p = s;
          size_t tally = 0;
          while (*p) {
            tally *= 17;
            tally += *p;
            p++;
          }
          return tally;
        }
    };
  
  /* FIXME: The following are just temporary workarounds.  In the long run we should
   * implement a hashtable and do things properly. */
  /* Approximate a hash_map with just a map */
  template <typename Key, typename Value, typename HashFunction=hash<Key> >
    class hash_map {
      public:
        typedef typename std::map<Key, Value>::iterator iterator;
        typedef typename std::map<Key, Value>::size_type size_type;
      private:
        std::map<Key, Value> internalMap;
      public:
        inline Value& operator[](const Key& key) { return internalMap[key]; }
        inline iterator begin() { return internalMap.begin(); }
        inline iterator end() { return internalMap.end(); }
        inline iterator find(const Key& key) { return internalMap.find(key); }
        inline void erase(iterator pos) { internalMap.erase(pos); }
        inline size_type erase(const Key& key) {
          return internalMap.erase(key);
        }
        inline size_type count(const Key& key) {
          return internalMap.count(key);
        }
    };

  /* TODO: finish the following skeleton */
	template <typename Key, typename Value, typename HashFunction=hash<Key> >
		class hash_set {
			public:
				typedef typename std::map<Key, Value>::iterator iterator;
        typedef typename std::map<Key, Value>::size_type size_type;
			private:
				std::set<Key, Value> internalSet;
			public:
		};
}
#endif //0
#endif // __GNUC__

#endif // GNU_EXTENSIONS_H

